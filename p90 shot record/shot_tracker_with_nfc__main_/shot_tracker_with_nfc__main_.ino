#include <LiquidCrystal_I2C.h>


LiquidCrystal_I2C lcd_1(0x27, 16, 2);


const int chkReload = 8; //records reloads when this is HIGH
const int shoot = 7; //Records shot when this is HIGH
const int mag = 50; //ammo capacity

void setup()
{
  lcd_1.init();
  lcd_1.setBacklight(1);
  pinMode(chkReload, INPUT);//sets the shoot and reload check pins to input
  pinMode(shoot, INPUT);
  
  Serial.begin(9600);
  Serial.print("hello World"); //don't @ me
}


void loop() //BUG currently when on first loop after boot, it just resets number back to 50 at a random point but never again
{
  bool reload;
  int tmp = 0;
  
  
  
  for (int ammo = mag; i>0;)//process for magazine counter
  {
      tmp = fire(i); //calls function for record of shot. Leave the tmp alone
      ammo = tmp;
      printAmount(ammo);

    else
    { 
    Serial.println("Error at tickdown. Below should match"); 
    Serial.println("Ammo in Mag: ");
    Serial.print(ammo); 
    Serial.println("Shoot tracker: ");
    Serial.print(i); 
    lcd_1.print("Error, see Serial");
      while(true);
     }
   
  
  printAmount(ammo); //displays ammo remaining after fired
  delay(50); //stops from running too fast. By tick is bad idea
  if(ammo == 0) //when ammo 0, starts reload process
  {
   reload = true; //true in this case is magazine in place or there. False is no magazine
   lcd_1.clear();
   lcd_1.print("Reload"); //signal to reload
    
   while(reload) //stops everything until reloaded.
     {       
     if (digitalRead(chkReload) == HIGH) //reads pin and sets reload when high. High is mag in gun
     {
       reload = false;
       while(digitalRead(chkReload) == HIGH) { } //wait for signal to turn off and turn back on before the while(reload) checks again. This is because it takes time to reload
      }
     }
  
   }
  }
  
  lcd_1.clear();//clears screen when reloaded. loops starts again
}


void printAmount(int count)
{
  lcd_1.clear(); //clears output before display. This prevents numbers blow 10 to have a zero after then. e.g. 90
  lcd_1.setCursor(0, 1);
  lcd_1.print(count);
}


int fire(int i) //lower one at a time
{  
  
  while(true) //Locks it and waits for input. Doesn't matter that it while true becuase of delay and ends when high anyway
  {
    delay(50);
    if(digitalRead(shoot) == HIGH) //the input
    {
      int tmp = i; //better to have temp then not at all
      tmp--;
      //printAmount(i);  //debugging just for vision on when it changes
      return tmp;
    }
    else
    {
      Serial.println("no shoot");
    }
  }
}
   
