#define DECODE_NEC  //Must be first

//don't define anything else for IR as I edited "PinDefinitionsAndMore.h" to change the IR_receive_pin from 2 to A2
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <IRremote.hpp>  // This include defines the actual pin number for pins like IR_RECEIVE_PIN, IR_SEND_PIN for many different boards and architectures
#include <Arduino.h>
#include <string.h>
#include <avr/wdt.h>
#include <EEPROM.h>
#include <math.h>
#include <Fonts/FreeSans9pt7b.h>

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels

#define OLED_DC 9
#define OLED_CS 10
#define OLED_RESET -1


Adafruit_SSD1306 lcd_1(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, OLED_DC, OLED_RESET, OLED_CS);

const float batMaxVolt = 8.4;         //9.80 is the max voltage of the NiMF bat
const int16_t BAT_TOTAL_HEIGHT = 16;  //the box must be 16bytes long and basically just pasting the 1s onto the screen.
const int16_t BAT_TOTAL_WIDTH = 16;
const int8_t BAT_WOFFSET = 5;  //offset of the batter itself from the 16 bit screen do -1 for the bottom
const int8_t BAT_X = 3;        //battery x position
const float minBat = 6.9;
const int8_t BAT_WIDTH = 8;                                  //width of internal volume
const int8_t BAT_HEIGHT = 10;                                //height of internal volume
const int16_t BAT_Y = lcd_1.height() - (BAT_WIDTH * 2) - 3;  //the Y position of the top of the full battery image
static const unsigned char PROGMEM batLogo[] = { 0b00000000, 0b00000000,
                                                 0b00000011, 0b11000000,
                                                 0b00001111, 0b11110000,
                                                 0b00001000, 0b00010000,
                                                 0b00001000, 0b00010000,
                                                 0b00001000, 0b00010000,
                                                 0b00001000, 0b00010000,
                                                 0b00001000, 0b00010000,
                                                 0b00001000, 0b00010000,
                                                 0b00001000, 0b00010000,
                                                 0b00001000, 0b00010000,
                                                 0b00001000, 0b00010000,
                                                 0b00001111, 0b11110000,
                                                 0b00000000, 0b00000000,
                                                 0b00000000, 0b00000000,
                                                 0b00000000, 0b00000000 };

const int16_t chkReload = 11;  //records reloads when this is HIGH
const int16_t highAnal = 110;  //any signal over is counted as a shot. Light level

const int16_t readVoltage = A1;
const int8_t textSize = 2;
const int16_t eeAddress = 0;  //address of mag
unsigned long last_time;
const int16_t remote[] = {
  0x16,  //zero 0
  0xC,   //one 1
  0x18,  //two 2
  0x5E,  //three 3
  0x8,   //four 4
  0x1C,  //five 5
  0x5A,  //six 6
  0x42,  //seven 7
  0x52,  //eight 8
  0x4A,  //nine 9
  0xD,   //clear 10
  0x19,  //down 11
  0x45,  //Power 12
  0x47,  //Menu 13
  0x44,  //Test 14
  0x40,  //up 15
  0x43,  //back 16
  0x7,   //left 17
  0x15,  //confirm 18
  0x9,   //right 19
};

const String menuItems[]{
  "0 Exit",
  "1 Set Ammo",
  "2 set EEPROM",
  "3 load EEPROM",
};

struct resources {
  int16_t keyPressed;
  int ammo;
  float magazine;
  int16_t batLastVal;
  int8_t refreshCounter;
};

struct resources box;
//____________________________________

void setup() {  //https://electronoobs.com/eng_arduino_tut132.php - example but is made for Atmega328P

  PCICR |= (1 << PCIE0);    //enable PCMSK0 scan.
  PCMSK0 |= (1 << PCINT3);  //Set pin MISO to react to interrupts. https://medesign.seas.upenn.edu/index.php/Guides/MaEvArM-pcint
  PCMSK0 |= (1 << PCINT7);  //ping 11
  DDRB &= B01110111;        //set only pin PB7 (D11) and PB3 (MISO) as input. Both these pins are on port B
  //other ways to do above
  //end of digitalPin Alternative Initialisation

  //setup main tools
  lcd_1.begin(SSD1306_SWITCHCAPVCC);
  lcd_1.display();  //plays logo. Requirement for using lib is to do so
  delay(2000);      // Pause for 2 seconds
  lcd_1.clearDisplay();
  lcd_1.setRotation(2);  //flips 180" (0=0, 1=90, 2=180, 3=270)
  lcd_1.setTextSize(textSize);
  lcd_1.setTextColor(SSD1306_WHITE);
  lcd_1.setFont(&FreeSans9pt7b);
  // Pin 2 and is hard coded in library so I changed it to A2
  IrReceiver.begin(A2);

  last_time = micros();
  //set ammo
  loadEE();
  if (!box.magazine)  //if boots and magazine empty. Ask to set
  {
    setAmmo();
  }
  box.ammo = box.magazine;

  //create first screen
  printAmount();
  createBat();
}

//____________________________________

void saveEE() {  //save magazine to an address in EEPROM
  //eeAddress += sizeof(float); //Move address to the next byte after previous entry.
  EEPROM.put(eeAddress, box.magazine);  //data must be a float
}

//____________________________________

void loadEE() {  //saved values into magazine
  EEPROM.get(eeAddress, box.magazine);
}

//____________________________________

void reboot() {  //reboots device
  wdt_disable();
  wdt_enable(WDTO_15MS);
  while (1) {}
}

//____________________________________

void clearInput() {  //if set to NULL then would be treated as 0
  box.keyPressed = 9001;
}
//____________________________________

void createBat() {  //create and display the empty battery. Call fillBat
  lcd_1.drawBitmap(BAT_X - 6, BAT_Y, batLogo, BAT_TOTAL_WIDTH, BAT_TOTAL_HEIGHT, SSD1306_WHITE);
  fillBat();  //create then fill
}
//____________________________________
void fillBat() {                                                                                                                                                                                                      //fills and displays the full battery
  lcd_1.fillRect(BAT_X - 1, BAT_Y - BAT_WOFFSET - 2 + BAT_HEIGHT /* goto height position-offset from 16-2and + internal height to set move it to the top of the bottom*/, BAT_WIDTH - 2, BAT_HEIGHT, SSD1306_WHITE);  //fill the battery internals box. Used because it distinguishes the empty and full batteries without using 2 bitmaps                                                                                                                                                                                                            //print to screen
}
//____________________________________
void drainBat() {
  float volt = (getVoltage() - minBat) / (batMaxVolt - minBat) * 100;  //calculates the battery percentage and takes into consideration that the lowest the battery can go is 6.5V
  float thresh = 100 / BAT_HEIGHT;                                     //BAT_HEIGHT is 9 and this gets the number of 9's in 100. Its 11.11
  float invert = 100 - volt;                                           //takes voltage percentage off 100, inverting it.
  float pipNum = invert / thresh;
  //how many theshholds are in the inverted value. e.g. if invert is 20 and thresh is 11, then this is how many 11s in 20.
  //As invert is created from missing percentage and thresh is how many 9s in 100, then this should return how many battery pips have been used. This can then be taken off the BAT_HEIGHT
  int32_t final_val = round(pipNum);  //rounds to the nearest whole number as is safer when the battery is low (gives more time by being slightly wrong)

  lcd_1.fillRect(BAT_X - 1, BAT_Y + BAT_WOFFSET - 1, BAT_WIDTH - 2, final_val, SSD1306_INVERSE);  //creates a box at the top of bat and this is made of inverse colour. This gets bigger as it drains and grows downwards. should end at BAT_HEIGHT
  //this is accurate but remember that the voltage read isn't always accurate. ATM it is 0.5v lower than real
}

//____________________________________

float getVoltage() {  //gets battery voltage
  //this assumes the input voltage is being converted from a higher voltage to 5v. if input is already <=5v then remove section involving the result variable
  // keep an eye out for noise on that pin (changing values)
  //if noise is minimal and the input set correctly, this should be accurate to the 2nd decimal point.
  float inputRef = 5.38;  //5.02this varies from device to device. on ATMEGA32U4, measure voltage of pin 24 (AVCC) on the chip and use that here.
  float in = (float)analogRead(readVoltage);
  float voltage = in * (inputRef / 1023.0);  //1023 is the steps value for a 10 bit A/D converter. inputRef is the max voltage for the AVCC pin on the ATMEGA32U4, I just measured the out voltage of pin24 on the chip. These 2 are what makes the voltage accurate as some chips might not have a 10bit A/D converter or have 4.25 as max
  //source https://www.youtube.com/watch?v=gw72g4WBz-U
  //power supply can be noisy and will affect accuracy of this reader as the basis this reader uses to measure is that input voltage (measured from AVCC by default)

  //to restore the original voltage before conversion
  float result = voltage * 5;  //the times value is the input voltage / output of conversion


  if (result <= minBat)  //if min voltage reached
  {
    lcd_1.clearDisplay();
    lcd_1.setTextSize(1);
    lcd_1.setCursor(0, lcd_1.height() - 30);
    lcd_1.print("Please Turn Off");
    lcd_1.display();
    while (true) {}  //never end and force it to be powered off
  }
  return result;
}
//____________________________________
void irRemote() {  //scans for input from remote and saves
  /*
     * Check if received data is available and if yes, try to decode it.
     * Decoded result is in the IrReceiver.decodedIRData structure.
     * Current Remote Key hex (missing the 0x ofc, e.g. 0x5E):
     */
  clearInput();
  if (IrReceiver.decode()) {
    size_t size = sizeof(remote) / sizeof(remote[0]);  //gets size of array remote
    //ONCE MADE AN ARRAY, TRY TO DO THIS AS A FOR LOOP. MAYBE ANOTHER ARRAY THAT INCLUDES THE ACTUAL COMMANDS? I ARRAY THAT CONTAINS THE DATA CHECKED AND ANOTHER THAT INCLUDED THE CORRESPONDING COMMAND
    for (int16_t i = 0; i < size; i++) {                    //cycles through whole document until hit value it needs
      if (IrReceiver.decodedIRData.command == remote[i]) {  //compared the command (hex value of the button) to the array
        box.keyPressed = i;                                 //records the point in array and not value
        break;                                              //break for loop
      }
    }
    IrReceiver.resume();  // Enable receiving of the next value
  }
}
//____________________________________

void printAmount() {  //print current mag count

  lcd_1.clearDisplay();
  lcd_1.setTextSize(textSize);
  lcd_1.setCursor(70, 50);
  lcd_1.setFont(&FreeSans9pt7b);
  lcd_1.print(box.ammo);

  createBat();
  drainBat();
  refreshDis();
}
//____________________________________

void initializeMenu() {  //create the menu list
  lcd_1.clearDisplay();
  size_t size = sizeof(menuItems) / sizeof(menuItems[0]);  //gets size of array remote
  for (int16_t i = 0; i < size; i++) {
    lcd_1.setCursor(25, 10 * i);  //the times I is because the value should get bigger every loop in relation to i
    lcd_1.setTextSize(1);
    lcd_1.print(menuItems[i]);
  }
  refreshDis();
}

void setAmmo() {  //sets ammo and saves to EEPROM
  bool clrBtn = true;
  int16_t x = 25;
  delay(250);
  while (clrBtn)  //this is so if the clear button pressed, if not cleared then this should never loop
  {
    int16_t sizeInput = 2;
    int16_t input[sizeInput];
    int16_t mag = 0;
    int16_t count = 0;
    lcd_1.clearDisplay();
    lcd_1.setCursor(x, 5);
    lcd_1.setTextSize(1);
    lcd_1.print("Enter mag size");
    lcd_1.setCursor(x, lcd_1.height() - 40);  //50 from the bottom
    lcd_1.drawFastHLine(x, lcd_1.height() - 25, lcd_1.width() - 70, SSD1306_WHITE);
    refreshDis();

    while (clrBtn) {
      irRemote();
      if (box.keyPressed <= 9 && count < sizeInput) {  //character limit Must be 2
        input[count] = box.keyPressed;                 //if the values
        lcd_1.setTextSize(1);
        lcd_1.print(input[count]);
        count++;

      } else if (box.keyPressed == 16)  //if back pressed
      {
        clrBtn = false;  //stops both loops so it can go back to main menu. Count set to 0 so things skipped
        count = 0;
      } else if (box.keyPressed == 10)  //if clear pressed
      {
        count = 0;
        //clear btn. Let it loop through so the while loop starts again
      } else if (box.keyPressed == 18 && count == sizeInput)  //confirm. If input array full
      {
        clrBtn = false;  //break both loops
      }
      refreshDis();
    }


    if (count != 0 && clrBtn == false)  //if the above section was skipped then count will still be 0. If confirm pressed, then count would not be
    {
      unsigned pow = 10;
      while (input[sizeInput - 1] >= pow)
        pow *= 10;
      mag = input[0] * pow + input[sizeInput - 1];
      //only works with a size of 2

      //Confirm Section
      lcd_1.clearDisplay();
      lcd_1.setCursor(5, 5);
      lcd_1.setTextSize(1);
      box.magazine = mag;

      lcd_1.print("mag is now ");
      lcd_1.print(box.magazine);
      refreshDis();
      delay(2000);  //leave it on screen
    }
  }
}

void menu() {
  lcd_1.setFont(NULL);  //reset font
  bool breakL = true;
  initializeMenu();  //display the menu items

  while (breakL) {
    irRemote();
    switch (box.keyPressed) {
      case 0:  //break loop and menu
        breakL = false;
        break;
      case 1:
        setAmmo();
        breakL = false;
        box.ammo = box.magazine;  //resets ammo count
        saveEE();                 //saves to EEPROM
        break;
      case 2:
        //save
        saveEE();
        break;
      case 3:
        //load
        loadEE();
        break;
      case 12:
        //reboot via power button
        reboot();
        break;
    }
  }
}
//_________________________________________

void refreshDis() {
  box.refreshCounter = 0;  //to prevent unessasary refreshes
  lcd_1.display();
}

//_________________________________________

void loop() {  //loop must always be cycling

  irRemote();  //run IR first
  switch (box.keyPressed) {
    case 13:  //menu
      menu();
      printAmount();
      break;
    case 12:  //Power
      reboot();
      break;
  };

  box.refreshCounter++;
  if (box.refreshCounter >= 2000) {  //display needs to update every once in a while for the battery drain to update but always updating is an issue
    printAmount();
    drainBat();
  }
}

//keep interrupts simple and quick. Port manipulation is best used here as faster then digitalReads (about 60x faster) but Best to only use in interrupts
//source:
//https://electronoobs.com/eng_arduino_tut166_code1.php - Chronograph code
//https://www.flickr.com/photos/28521811@N04/8467610175 - pinout for the atmega32u4
//https://electronoobs.com/eng_arduino_tut132.php -- Pin change interrupts but for Atmega328P
//https://medesign.seas.upenn.edu/index.php/Guides/MaEvArM-pcint - atmega32u4 pin change interrupts
//https://electronoobs.com/eng_arduino_tut130.php - Port Register Control. Faster then digitalRead

ISR(PCINT0_vect) {                                        //Pin change interrupt (no pin HIGH/LOW interrupt this fast on ATmega). Even though PCINT3 is being scanned. Only 0 will trigger. Any PCINTn will trigger this funtion. Can do do internal checks regarding this
  if (PINB & B00001000 && (micros() > last_time + 10)) {  //Reading pin 3 (PB3) directly from port B, This should be the interupt pin PCINT3. Confirms which pin is in use at the moment

    if (box.ammo > 0)  //process for magazine counter. Simple process. Count and stop when mag capacity reached
    {
      box.ammo--;
      printAmount();
      last_time = micros();
    }

    if (box.ammo == 0) {
      lcd_1.clearDisplay();
      lcd_1.setTextSize(1);
      lcd_1.setCursor(60, 57);
      lcd_1.print(F("Reload"));  //signal to reload
      createBat();
      drainBat();
      refreshDis();
    }
  }
  //End of PCINT3 Interrupt Trigger
  //Start of PCINT7 Interrupt (Reload button)
  if (!(PINB & B10000000))  // if pin PB7 of port B (D11) is false (I not sure why either but it works). Confirms which pin is in use at the moment
  {
    box.ammo = box.magazine;
    printAmount();
  }  //Reload being an interrupt is good as I can reload whenever
}  //This interrupt may be big but only code executed is that of the specific interrupt that called it. Hense, It never fully being executed, at once