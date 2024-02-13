#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels


#define OLED_RESET     -1 // Resets when arduino resets. The reset pin is -1
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int chkReload = 15; //records reloads when this is HIGH
const int shoot = 14; //Records shot when this is HIGHpin 6 and 7 broke
const int mag = 50; //ammo capacity
const int highAnal = 110;
const int analogPin = A0;

void setup()
{
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  display.setCursor(0,17); //not standard display so 17 pixels down as compensation
  display.display();
  delay(2000); // Play splash screen and Pause for 2 seconds

  
  pinMode(chkReload, INPUT);//sets the shoot and reload check pins to input
  pinMode(shoot, INPUT);

  /*Serial.begin(9600);
  Serial.print("hello World"); //don't @ me*/
}


int fire(int i) //lower one at a time
{
  int tmp = i; //better to have temp then not at all
  
  while (true) //Locks it and waits for input. Doesn't matter that it while true becuase of delay and ends when high anyway
  {
    int value = 0;//reset
    value = analogRead(analogPin);//constantly reads pin and will then check for any flash of light
    int convertValue = value/4; //converts the analog value (0-1023) to digital 0-255
    
    if (convertValue >= highAnal) //if the input is brighter then the background. This will be 30-60 while in the tracer. UV shoots it over 200
    {
      tmp--;
      return tmp;//breaks function (good thing)
   }
    else
    {
      //No shoot
    }
    delay(2);//no shoot state would use up too many resources otherwise
  }
}

void printAmount(int count)
{//basically refreshes the display with Ammo: [value]
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,17);
  display.print(F("Ammo: "));
  display.display();
  display.print(count);
  
  display.display();
}


void loop() //BUG currently when on first loop after boot, it just resets number back to 50 at a random point but never again
{
  bool reload;
  int tmp = 0;
  int ammo;
  int lastState;
  for (ammo = mag; ammo > 0;) //process for magazine counter. Simple process to loop, count and stop when mag capacity reached
  {
    
    printAmount(ammo);
    tmp = fire(ammo); //calls function for record of shot. Leave the tmp alone
    ammo = tmp;
    printAmount(ammo);
    delay(150); //delay after shot to avoid double fires from the light still being on
  }

    delay(50); //stops from running too fast. By tick is bad idea
    if (ammo == 0) //when ammo 0, starts reload process
    {
      reload = true; //true in this case is magazine in place or there. False is no magazine
      display.clearDisplay();
      display.setCursor(0,17);
      display.print(F("Reload")); //signal to reload
      display.display(); //Prints to display

      while (reload) //stops everything until reloaded.
      {
        if (digitalRead(chkReload) == HIGH) //reads pin and sets reload when high. High is mag in gun
        {
          reload = false;
          while (digitalRead(chkReload) == HIGH) { } //wait for signal to turn off and turn back on before the while(reload) checks again. This is because it takes time to reload
        }
      }
    }
  
  display.clearDisplay();
}
