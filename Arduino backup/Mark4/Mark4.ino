#include <LiquidCrystal.h>
const int sensorPin = A0;
const int baseTemp = 20.0;
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);



void setup() {

  lcd.begin(16, 2);
  lcd.print("hello, world!");
  delay(1200);
  
}


void loop() {
  

  int sensorVal = analogRead(sensorPin);
  float voltage = (sensorVal/1024.0) * 5.0;
  float temp = (voltage - .5) * 100;
 
  lcd.setCursor(0, 0);
   lcd.print(temp);
   lcd.print(" 'C       ");
  delay(1000);
}
