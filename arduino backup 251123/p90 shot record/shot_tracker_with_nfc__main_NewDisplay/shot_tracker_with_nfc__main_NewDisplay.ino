#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32


const int chkReload = 15; //records reloads when this is HIGH
const int mag = 50; //ammo capacity
const int highAnal = 110;  //any signal over is counted as a shot. Light level
const int analogPin = 18; //Actually A0 but used as digital

Adafruit_SSD1306 lcd_1(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup()
{
  lcd_1.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  lcd_1.display(); //plays logo. Requirement for using lib is to do so
  delay(2000); // Pause for 2 seconds
  lcd_1.clearDisplay();
  lcd_1.setRotation(2); //flips 180" (0=0, 1=90, 2=180, 3=270)


  lcd_1.setTextSize(2);
  lcd_1.setTextColor(SSD1306_WHITE);

  
  pinMode(chkReload, INPUT);//sets the reload check pins to input
  pinMode(analogPin, INPUT);

  Serial.begin(9600);
  Serial.print("hello World"); //don't @ me
}


int fire(int i) //lower one at a time
{
  int tmp = i; //better to have temp then not at all
  
  while (true) //Locks it and waits for input. Doesn't matter that it while true becuase of delay and ends when high anyway
  {
    int value = 0;//reset
    value = digitalRead(analogPin);//constantly reads pin and will then check for any flash of light
    
    if (value) //if the input is brighter then the background. This will be 30-60 while in the tracer. UV shoots it over 200
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
{
  lcd_1.clearDisplay();

  lcd_1.setCursor(32,0); 
  lcd_1.print(F("Ammo: "));
  lcd_1.display();

  lcd_1.print(count);
  lcd_1.display();
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
      lcd_1.clearDisplay();
      lcd_1.setCursor(32,0); 
      lcd_1.print(F("Reload")); //signal to reload
      lcd_1.display();

      while (reload) //stops everything until reloaded.
      {
        if (digitalRead(chkReload) == HIGH) //reads pin and sets reload when high. High is mag in gun
        {
          reload = false;
          while (digitalRead(chkReload) == HIGH) { } //wait for signal to turn off and turn back on before the while(reload) checks again. This is because it takes time to reload
        }
      }
    }
  
  lcd_1.clearDisplay();//clears screen when reloaded. loops starts again
}
