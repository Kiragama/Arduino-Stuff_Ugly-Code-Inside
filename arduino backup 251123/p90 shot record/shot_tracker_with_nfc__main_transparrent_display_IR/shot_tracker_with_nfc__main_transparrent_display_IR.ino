#define DECODE_NEC       //Must be first

//don't define anything else for IR as I edited "PinDefinitionsAndMore.h" to change the IR_receive_pin from 2 to A2
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "PinDefinitionsAndMore.h"
#include <IRremote.hpp> // This include defines the actual pin number for pins like IR_RECEIVE_PIN, IR_SEND_PIN for many different boards and architectures
#include <Arduino.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_DC     9
#define OLED_CS     10
#define OLED_RESET  -1
Adafruit_SSD1306 lcd_1(SCREEN_WIDTH, SCREEN_HEIGHT,
  &SPI, OLED_DC, OLED_RESET, OLED_CS);

const bool invert = true;
const int chkReload = 11; //records reloads when this is HIGH
const int mag = 50; //ammo capacity
const int highAnal = 110;  //any signal over is counted as a shot. Light level
const int analogPin = A0;

const int r_Power = 0x45;
const int r_Menu = 0x47;
const int r_Test = 0x44;
const int r_Up = 0x40;
const int r_Back = 0x43;
const int r_Left = 0x7;
const int r_Play = 0x15;
const int r_Right = 0x9;
const int r_Zero = 0x16;
const int r_One = 0xC;
const int r_Two = 0x18;
const int r_Three = 0x5E;
const int r_Four = 0x8;
const int r_Five = 0x1C;
const int r_Six = 0x5A;
const int r_Seven = 0x42;
const int r_Eight = 0x52;
const int r_Nine = 0x4A;
const int r_Clear = 0xD;
const int r_Down = 0x19;
//MAKE THIS AN ARRAY

void setup()
{
  lcd_1.begin(SSD1306_SWITCHCAPVCC);
  lcd_1.display(); //plays logo. Requirement for using lib is to do so
  delay(2000); // Pause for 2 seconds
  lcd_1.clearDisplay();
  lcd_1.setRotation(2); //flips 180" (0=0, 1=90, 2=180, 3=270)

  lcd_1.setTextSize(1);
  lcd_1.setTextColor(SSD1306_WHITE);
  lcd_1.dim(true);

  
  pinMode(chkReload, INPUT);//sets the reload check pins to input
  pinMode(analogPin, INPUT);
  //pinMode(confirmPin, INPUT);

  Serial.begin(9600);
  Serial.print("hello World"); //don't @ me
  // Pin 2 and is hard coded in library so I changed it to A2
  IrReceiver.begin(IR_RECEIVE_PIN);
}

//IR
void irRemote ()
{
   /*
     * Check if received data is available and if yes, try to decode it.
     * Decoded result is in the IrReceiver.decodedIRData structure.
     * Current Remote Key hex (missing the 0x ofc, e.g. 0x5E):
     */
   if (IrReceiver.decode()) {
        //IrReceiver.printIRSendUsage(&Serial); //print whole results
        //Serial.println(IrReceiver.decodedIRData.command, HEX); //print command in hex
        
        //ONCE MADE AN ARRAY, TRY TO DO THIS AS A FOR LOOP. MAYBE ANOTHER ARRAY THAT INCLUDES THE ACTUAL COMMANDS? I ARRAY THAT CONTAINS THE DATA CHECKED AND ANOTHER THAT INCLUDED THE CORRESPONDING COMMAND
        switch(IrReceiver.decodedIRData.command){ //if key pressed then do something
          case r_Up: //up
          Serial.println("up");
          break;
          
          case r_Down: //down
          Serial.println("down");
          break;

          case r_Left: //left
          Serial.println("left");
          break;

          case r_Right: //right
          Serial.println("right");
          break;

          case r_Zero: //0
          Serial.println("0");
          break;

          case r_One: //1
          Serial.println("1");
          break;

          case r_Two: //2
          Serial.println("2");
          break;

          case r_Three://3
          Serial.println("3");
          break;

          case r_Four://4
          Serial.println("4");
          break;

          case r_Five://5
          Serial.println("5");
          break;

          case r_Six://6
          Serial.println("6");
          break;

          case r_Seven://7
          Serial.println("7");
          break;

          case r_Eight://8
          Serial.println("8");
          break;

          case r_Nine://9
          Serial.println("9");
          break;

          case r_Clear://Clear
          Serial.println("Clear");
          break;

          case r_Power://Power
          Serial.println("Power");
          break;

          case r_Menu://Menu
          Serial.println("Menu");
          break;

          case r_Back://Back
          Serial.println("Back");
          break;

          case r_Test://Test
          Serial.println("Test");
          break;

          case r_Play://Play
          Serial.println("Play");
          break;

          default:
          Serial.println("Bad Read");
          break;
        }
        delay(100);
        IrReceiver.resume(); // Enable receiving of the next value
    }
}


int fire(int i) //lower one at a time
{
  int tmp = i; //better to have temp then not at all
  
  while (true) //Locks it and waits for input. Doesn't matter that it while true becuase of delay and ends when high anyway
  {
    int value, confirmVal = 0;//reset
    value = digitalRead(analogPin);//constantly reads pin and will then check for any flash of light
    //confirmVal = digitalRead(confirmPin); //same as above but reads a seperate sensor. Will run an either or situation and use delay to stop duplicate shots
    if (value) //if the input is brighter then the background. This will be 30-60 while in the tracer. UV shoots it over 200
    {
      tmp--;
      return tmp;//breaks function (good thing)
    }
    else
    {
      //No shoot
    }
    //delay(1);//no shoot state would use up too many resources otherwise
  }
}

void printAmount(int count, bool invert)
{

  lcd_1.clearDisplay();

  lcd_1.setCursor(110,57); 
  lcd_1.display();
  lcd_1.print(count);

  
  lcd_1.display();
}






void loop() //BUG currently when on first loop after boot, it just resets number back to 50 at a random point but never again
{
  irRemote(); //run IR first

  bool reload;
  int tmp = 0;
  int ammo;
  int lastState;
  for (ammo = mag; ammo > 0;) //process for magazine counter. Simple process to loop, count and stop when mag capacity reached
  {
    
    printAmount(ammo, invert);
    tmp = fire(ammo); //calls function for record of shot. Leave the tmp alone
    ammo = tmp;
    printAmount(ammo, invert);
    delay(150); //delay after shot to avoid double fires from the light still being on
  }
    delay(50); //stops from running too fast. By tick is bad idea
    if (ammo == 0) //when ammo 0, starts reload process
    {
      reload = true; //true in this case is magazine in place or there. False is no magazine
      lcd_1.clearDisplay();
      lcd_1.setCursor(85,57); 
      lcd_1.print(F("Reload")); //signal to reload
      lcd_1.display();

      while (digitalRead(chkReload) == HIGH) //stops everything until reloaded.
      {
       /* if (digitalRead(chkReload) == HIGH) //reads pin and sets reload when high. High is mag in gun
        {
          reload = false;
          //while (digitalRead(chkReload) == HIGH) { } //wait for signal to turn off and turn back on before the while(reload) checks again. This is because it takes time to reload
        }
        delay(500);
        if (digitalRead(chkReload) == HIGH) //reads pin and sets reload when high. High is mag in gun
        {
          reload = true;
          //while (digitalRead(chkReload) == HIGH) { } //wait for signal to turn off and turn back on before the while(reload) checks again. This is because it takes time to reload
        }*/
      }
    }
  
  lcd_1.clearDisplay();//clears screen when reloaded. loops starts again
}
