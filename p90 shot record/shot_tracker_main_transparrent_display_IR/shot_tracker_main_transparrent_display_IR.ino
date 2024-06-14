#define DECODE_NEC  //Must be first

//don't define anything else for IR as I edited "PinDefinitionsAndMore.h" to change the IR_receive_pin from 2 to A2
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <IRremote.hpp>  // This include defines the actual pin number for pins like IR_RECEIVE_PIN, IR_SEND_PIN for many different boards and architectures
#include <Arduino.h>
#include <string.h>
//#include <avr/wdt.h>

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels

#define OLED_DC 7
#define OLED_CS 0
#define OLED_RESET 5
Adafruit_SSD1306 lcd_1(SCREEN_WIDTH, SCREEN_HEIGHT,
                       &SPI, OLED_DC, OLED_RESET, OLED_CS);

const int chkReload = 11;  //records reloads when this is HIGH
const int highAnal = 110;  //any signal over is counted as a shot. Light level
const int analogPin = A0;



const int remote[] = {
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
  "4 clear EEPROM"
};

struct resources {
  int keyPressed;
  int ammo;
  int magazine;
};

struct resources box;

//____________________________________

void setup() {
  lcd_1.begin(SSD1306_SWITCHCAPVCC);
  lcd_1.display();  //plays logo. Requirement for using lib is to do so
  delay(2000);      // Pause for 2 seconds
  lcd_1.clearDisplay();
  lcd_1.setRotation(2);  //flips 180" (0=0, 1=90, 2=180, 3=270)

  lcd_1.setTextSize(1);
  lcd_1.setTextColor(SSD1306_WHITE);
  lcd_1.dim(true);
  box.magazine = 50;  //sets mag size
  box.ammo = box.magazine;
  printAmount();              //sets initial screen value
  pinMode(chkReload, INPUT);  //sets the reload check pins to input
  pinMode(analogPin, INPUT);
  //pinMode(confirmPin, INPUT);

  Serial.begin(9600);
  Serial.print("hello World");  //don't @ me
  // Pin 2 and is hard coded in library so I changed it to A2
  IrReceiver.begin(A2);
}

//____________________________________

void reboot() {
  //wdt_disable();
  //wdt_enable(WDTO_15MS);
  while (1) {}
}

//____________________________________

void clearInput() {
  box.keyPressed = 9001;  //if set to NULL then would be treated as 0
}
//____________________________________

//IR
void irRemote() {
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
    for (int i = 0; i < size; i++) {  //cycles through whole document until hit value it needs
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

void fire()  //lower one at a time
{
  int value;                       //reset
  value = digitalRead(analogPin);  //constantly reads pin and will then check for any flash of light
  if (value)                       //if the input is brighter then the background. This will be 30-60 while in the tracer. UV shoots it over 200
  {
    box.ammo--;
    printAmount();  //only update if there is a change
  } else {
    //No shoot
  }
  //delay(1);//no shoot state would use up too many resources otherwise
}
//____________________________________


void printAmount() {

  lcd_1.clearDisplay();

  lcd_1.setCursor(110, 57);
  lcd_1.print(box.ammo);

  lcd_1.display();
}

//____________________________________

void initializeMenu() {
  lcd_1.clearDisplay();
  size_t size = sizeof(menuItems) / sizeof(menuItems[0]);  //gets size of array remote
  for (int i = 0; i < size; i++) {
    lcd_1.setCursor(25, 10 * i);  //the times I is because the value should get bigger every loop in relation to i
    lcd_1.print(menuItems[i]);    //displays menu
  }
  lcd_1.display();
  Serial.println("initializeMenu");
}

void setAmmo() {
  bool clrBtn = true;
  int x = 25;
delay(250);
  while (clrBtn)  //this is so if the clear button pressed, if not cleared then this should never loop
  {
    int sizeInput = 2;
    int input[sizeInput];

    int mag = 0;
    int count = 0;
    lcd_1.clearDisplay();
    lcd_1.setCursor(x, 5);
    lcd_1.print("Enter mag size");
    lcd_1.setCursor(x, lcd_1.height() - 40);  //50 from the bottom
    lcd_1.drawFastHLine(x, lcd_1.height() - 38, lcd_1.width() - 70, SSD1306_WHITE);
    lcd_1.display();

    while (clrBtn) { //input[count] != NULL && count < 2
      //Serial.print("test");
      irRemote();
      if (box.keyPressed < 9 && count < sizeInput) { //character limit Must be 2
        input[count] = box.keyPressed;  //if the values
        Serial.print(input[count]);
        lcd_1.print(input[count]);
        count++;

      } else if (box.keyPressed == 16)  //if back pressed
      {
        clrBtn = false;                //stops both loops so it can go back to main menu. Count set to 0 so things skipped
        count = 0;
      } else if (box.keyPressed == 10)  //if clear pressed
      {
        count = 0;
        //clear btn. Let it loop through so the while loop starts again
      }
      else if (box.keyPressed == 18 && count == sizeInput -1) //confirm. If input array full
      {
        clrBtn = false;  //break both loops
      }
      lcd_1.display();
    }



    if (count != 0 && clrBtn == false)  //if the above section was skipped then count will still be 0. If confirm pressed, then count would not be 
    {
      //Confirm Section
      lcd_1.clearDisplay();
      lcd_1.setCursor(5, 5);
      box.magazine = mag;

      lcd_1.print("mag is now ");
      lcd_1.print(box.magazine);
      delay(2000); //leave it on screen
    }
  }
  Serial.print("endof ammo");
}

void menu() {
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
        break;
      case 2:
        //save
        break;
      case 3:
        //load
        break;
      case 4:
        //clear EE
        break;
    }
  }
  Serial.println("menu");
}

void loop()  //loop must always be cycling
{
  irRemote();  //run IR first
  menu();
  printAmount();  //debug

  if (box.keyPressed == 13)  //menu clicked
  {
    menu();
    printAmount();
  }

  bool reload;
  if (box.ammo > 0)  //process for magazine counter. Simple process to loop, count and stop when mag capacity reached
  {
    fire();      //calls function for record of shot. Leave the tmp alone
    delay(150);  //delay after shot to avoid double fires from the light still being on
  }

  if (box.ammo == 0)  //when ammo 0, starts reload process
  {
    reload = true;  //true in this case is magazine in place or there. False is no magazine
    lcd_1.clearDisplay();
    lcd_1.setCursor(85, 57);
    lcd_1.print(F("Reload"));  //signal to reload
    lcd_1.display();

    while (digitalRead(chkReload) == HIGH)  //stops everything until reloaded.
    {}
  }
}
