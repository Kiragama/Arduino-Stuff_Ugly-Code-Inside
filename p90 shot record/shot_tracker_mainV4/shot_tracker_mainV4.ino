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

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels

#define OLED_DC 9
#define OLED_CS 10
#define OLED_RESET -1
Adafruit_SSD1306 lcd_1(SCREEN_WIDTH, SCREEN_HEIGHT,
                       &SPI, OLED_DC, OLED_RESET, OLED_CS);

const float batMaxVolt = 18;        //9.80 is the max voltage of the NiMF bat
const int16_t BAT_TOTAL_HEIGHT = 16;  //the box must be 16bytes long and basically just pasting the 1s onto the screen.
const int16_t BAT_TOTAL_WIDTH = 16;
const int8_t BAT_WOFFSET = 4;  //offset of the batter itself from the 16 bit screen do -1 for the bottom
const int8_t BAT_X = 2;

const int8_t BAT_WIDTH = 6;                            //width of internal volume
const int8_t BAT_HEIGHT = 9;                           //height of internal volume
const int16_t BAT_Y = lcd_1.height() - (BAT_WIDTH * 2)-3;  //the Y position of the top of the full battery image
static const unsigned char PROGMEM batLogo[] = { 0b00000011, 0b11000000,
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
                                                 0b00000000, 0b00000000 };

const int16_t chkReload = 11;  //records reloads when this is HIGH
const int16_t highAnal = 110;  //any signal over is counted as a shot. Light level
const int16_t analogPin = A0;
const int16_t readVoltage = A1;

const int16_t eeAddress = 0;  //address of mag

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
  "4 clear EEPROM",
  "5 Reboot"
};

struct resources {
  int16_t keyPressed;
  int16_t ammo;
  float magazine;
  int16_t batLastVal;
  float lastVolt;
};

struct resources box;

//____________________________________

void setup() {

  //setup main tools
  lcd_1.begin(SSD1306_SWITCHCAPVCC);
  lcd_1.display();  //plays logo. Requirement for using lib is to do so
  delay(2000);      // Pause for 2 seconds
  lcd_1.clearDisplay();
  lcd_1.setRotation(2);  //flips 180" (0=0, 1=90, 2=180, 3=270)
  lcd_1.setTextSize(1);
  lcd_1.setTextColor(SSD1306_WHITE);
  lcd_1.dim(true);
  Serial.begin(9600);
  Serial.print("hello World");  //don't @ me
  // Pin 2 and is hard coded in library so I changed it to A2
  IrReceiver.begin(A2);
  pinMode(chkReload, INPUT);  //sets the reload check pins to input
  pinMode(analogPin, INPUT);
  //pinMode(confirmPin, INPUT);


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
  Serial.println("done");
}

//____________________________________

void loadEE() {  //saved values into magazine
  EEPROM.get(eeAddress, box.magazine);
  Serial.println(box.magazine, 3);  //This may print 'ovf, nan' if the data inside the EEPROM is not a valid float.
}

//____________________________________

void clearEE() {  //clear EEPROM
  for (int16_t i = 0; i < EEPROM.length(); i++) {
    EEPROM.write(i, 0);  //0 out EEPROM
  }
  reboot();
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
  lcd_1.drawBitmap(BAT_X, BAT_Y, batLogo, BAT_TOTAL_WIDTH, BAT_TOTAL_HEIGHT, SSD1306_WHITE);
  //lcd_1.display();
  fillBat();  //create then fill
}
//____________________________________
void fillBat() {                                                                                                                                                                                                               //fills and displays the full battery
  lcd_1.fillRect(BAT_X + BAT_WOFFSET + 1, BAT_Y - BAT_WOFFSET - 2 + BAT_HEIGHT /*goto height position-offset from 16-2and + internal height to set move it to the top of the bottom*/, BAT_WIDTH, BAT_HEIGHT, SSD1306_WHITE);  //fill the battery internals box. Used because it distinguishes the empty and full batteries without using 2 bitmaps
  //cd_1.display();                                                                                                                                                                                                             //print to screen
}
//____________________________________

int convertVolt() {                           //this gets the voltage, converts it to a value between 1 and 9 (BAT_HEIGHT) and makes a box of that size that expands down the battery icon. 0 in this case won't make a line
  float converted = batMaxVolt / BAT_HEIGHT;  //max voltage / the size of the battery box in pixels. This is to get the threshold for the battery ticking down
  float rounded = round(converted);           //round to the nearest whole number

  return rounded;  //return the threshold
}


//____________________________________
void drainBat() {
  float thresh = convertVolt();                 //gets the threshold
  Serial.println();
  Serial.println(thresh);
   
  float volt = getVoltage();                    //gets current voltage
  Serial.print("Voltage before convert is: ");  //debug
  Serial.println(volt);

  float voltDif = batMaxVolt - volt;             //take current voltage off the max to leave the difference between them
  float oldVoltDif = batMaxVolt - box.lastVolt;  //calculate the differnce for the old voltage aswell
  Serial.print("voltDif is");
  Serial.println(voltDif);
  Serial.print("oldVoltDif is");
  Serial.println(oldVoltDif);


  if (voltDif >= thresh && !(oldVoltDif >= thresh) && oldVoltDif != batMaxVolt)  //if difference value is greater then/equal the threshold value and not the same as the last one, then trigger.
  {
    if (box.batLastVal < BAT_HEIGHT) //stops it from going higher then the battery image. AKA so image drains fully
    {
    box.batLastVal++;  //add one to the last value, making the box bigger
    Serial.print("boxlastval is:");
    Serial.println(box.batLastVal);
    }
    else
    {
      reboot(); //well the battery is dead at this point
    }

    lcd_1.fillRect(BAT_X + 5, BAT_Y + BAT_WOFFSET - 1, BAT_WIDTH, box.batLastVal, SSD1306_INVERSE);  //creates a box at the top of bat and this is made of inverse colour. This gets bigger as it drains and grows downwards. should end at BAT_HEIGHT
    //lcd_1.display();
    box.lastVolt = volt;  //sets the current voltage for comparison on next call
  }
}

//____________________________________

float getVoltage() {  //gets battery voltage
  //||Details below will need to be tailored to the specific board||
  //this assumes the input voltage is being converted from a higher voltage to 5v. if input is already <=5v then remove section involving the result variable
  // keep an eye out for noise on that pin (changing values)
  //if noise is minimal and the input set correctly, this should be accurate to the 2nd decimal point.
  float inputRef = 5.02;  //this varies from device to device. on ATMEGA32U4, measure voltage of pin 24 (AVCC) on the chip and use that here.
  float in = (float)analogRead(readVoltage);
  float voltage = in * (inputRef / 1023.0);  //1023 is the steps value for a 10 bit A/D converter. inputRef is the max voltage for the AVCC pin on the ATMEGA32U4, I just measured the out voltage of pin24 on the chip. These 2 are what makes the voltage accurate as some chips might not have a 10bit A/D converter or have 4.25 as max
  //source https://www.youtube.com/watch?v=gw72g4WBz-U
  //power supply can be noisy and will affect accuracy of this reader as the basis this reader uses to measure is that input voltage (measured from AVCC by default)
  Serial.println(voltage);  //debug

  //to restore the original voltage before conversion
  float result = voltage * 5;  //the times value is the input voltage / output of conversion
                               //debug


  /* if (result <= 4.20)  //if min voltage reached
  {
    lcd_1.clearDisplay();
    lcd_1.setCursor(0, lcd_1.height() - 10);
    lcd_1.print("Battery low. Please power off");
    lcd_1.display();
    while (true) {}  //never end and force it to be powered off
  }*/
  return result;
}

//____________________________________
//IR
void irRemote() {  //scans for input from remote and saves
  /*
     * Check if received data is available and if yes, try to decode it.
     * Decoded result is in the IrReceiver.decodedIRData structure.
     * Current Remote Key hex (missing the 0x ofc, e.g. 0x5E):
     */
  clearInput();
  if (IrReceiver.decode()) {
    //IrReceiver.printIRSendUsage(&Serial); //print whole results
    //Serial.println(IrReceiver.decodedIRData.command, HEX); //print command in hex

    size_t size = sizeof(remote) / sizeof(remote[0]);  //gets size of array remote
    //ONCE MADE AN ARRAY, TRY TO DO THIS AS A FOR LOOP. MAYBE ANOTHER ARRAY THAT INCLUDES THE ACTUAL COMMANDS? I ARRAY THAT CONTAINS THE DATA CHECKED AND ANOTHER THAT INCLUDED THE CORRESPONDING COMMAND
    for (int16_t i = 0; i < size; i++) {  //cycles through whole document until hit value it needs
      if (IrReceiver.decodedIRData.command == remote[i]) {
        box.keyPressed = i;  //records the point in array and not value
        Serial.print(i);
        break;  //break for loop
      } else {
        Serial.println("Bad Read");
      }
    }
    delay(200);
    IrReceiver.resume();  // Enable receiving of the next value
  }
}

//____________________________________

void fire() {  //lower one at a time

  int16_t value;                   //reset
  value = digitalRead(analogPin);  //constantly reads pin and will then check for any flash of light
  if (!value)                       //if the input is brighter then the background. This will be 30-60 while in the tracer. UV shoots it over 200
  {
    box.ammo--;
    printAmount();  //only update if there is a change
  } else {
    //No shoot
  }
  //delay(1);//no shoot state would use up too many resources otherwise
}
//____________________________________


void printAmount() {  //print current mag count

  lcd_1.clearDisplay();

  lcd_1.setCursor(110, 57);
  lcd_1.print(box.ammo);

  
  createBat();
  drainBat();
  lcd_1.display();
}

//____________________________________

void initializeMenu() {  //create the menu list
  lcd_1.clearDisplay();
  size_t size = sizeof(menuItems) / sizeof(menuItems[0]);  //gets size of array remote
  for (int16_t i = 0; i < size; i++) {
    lcd_1.setCursor(25, 10 * i);  //the times I is because the value should get bigger every loop in relation to i
    lcd_1.print(menuItems[i]);    //displays menu
  }
  lcd_1.display();
  Serial.println("initializeMenu");
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
    lcd_1.print("Enter mag size");
    lcd_1.setCursor(x, lcd_1.height() - 40);  //50 from the bottom
    lcd_1.drawFastHLine(x, lcd_1.height() - 25, lcd_1.width() - 70, SSD1306_WHITE);
    lcd_1.display();

    while (clrBtn) {  //input[count] != NULL && count < 2
      //Serial.print("test");
      irRemote();
      if (box.keyPressed <= 9 && count < sizeInput) {  //character limit Must be 2
        input[count] = box.keyPressed;                 //if the values
        Serial.print(input[count]);
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
      lcd_1.display();
    }



    if (count != 0 && clrBtn == false)  //if the above section was skipped then count will still be 0. If confirm pressed, then count would not be
    {
      //conatenate (join to variable) input into mag
      //source: https://stackoverflow.com/questions/12700497/how-to-concatenate-two-integers-in-c
      unsigned pow = 10;
      while (input[sizeInput - 1] >= pow)
        pow *= 10;
      mag = input[0] * pow + input[sizeInput - 1];
      //only works with a size of 2

      //Confirm Section
      lcd_1.clearDisplay();
      lcd_1.setCursor(5, 5);
      box.magazine = mag;

      lcd_1.print("mag is now ");
      lcd_1.print(box.magazine);
      lcd_1.display();
      delay(2000);  //leave it on screen
    }
  }
  Serial.print("endof ammo");
}

void menu() {  //main menu
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
      case 4:
        //clear EE
        clearEE();
        break;
      case 12:
        //reboot via power button
        reboot();
        break;
      case 5:
        //reboot
        reboot();
        break;
    }
  }
  Serial.println("menu");
}

void loop() {  //loop must always be cycling


  irRemote();  //run IR first
  //menu();
  printAmount();  //debug
  switch (box.keyPressed) {
    case 13:  //menu
      menu();
      printAmount();
      break;
    case 12:  //Power
      reboot();
      break;
  };

  bool reload;
  if (box.ammo > 0)  //process for magazine counter. Simple process to loop, count and stop when mag capacity reached
  {
    fire();      //calls function for record of shot. 
    //delay(50);  //delay after shot to avoid double fires from the light still being on
  }

  if (box.ammo == 0)  //when ammo 0, starts reload process
  {
    reload = true;  //true in this case is magazine in place or there. False is no magazine
    lcd_1.clearDisplay();
    lcd_1.setCursor(85, 57);
    lcd_1.print(F("Reload"));  //signal to reload
    createBat();
    lcd_1.display();

    while (digitalRead(chkReload) == HIGH)  //stops everything until reloaded.
    {
      drainBat();
      delay(200);
    }

    //after reload pressed, reset mag
    box.ammo = box.magazine;
  }
  
}
