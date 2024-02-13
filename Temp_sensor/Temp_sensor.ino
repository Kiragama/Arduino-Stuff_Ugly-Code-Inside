#include <dht.h>
#include <LiquidCrystal.h>

//LiquidCrystal lcd(12,11,5,4,3,2); 
LiquidCrystal lcd(15,6,5,14,10,16); 
//initialise lcd variable to those pins

dht DHT;
int dhtPin = 9;

void setup() {
  Serial.begin(9600); //opens serial port
  lcd.begin(16, 2);
  lcd.print("BEEEEEEEEEE???!?");


}

void loop() {

 int chk = DHT.read11(dhtPin);
 switch (chk)
 {
  case DHTLIB_OK: //no problems
    //lcd.setCursor(0, 0);
    Serial.print("Humidity: ");
    Serial.print(DHT.humidity);
    Serial.print("%");
    //lcd.setCursor(0, 1);
    Serial.print("Temperature: ");
    Serial.print(DHT.temperature);
    Serial.print("'C");
    break;

  case DHTLIB_ERROR_CHECKSUM: //error
    Serial.print("Checksum error");
    break;

  case DHTLIB_ERROR_TIMEOUT: //guess
    Serial.print("Timeout error");
    break;
  default:
    Serial.print("Unknown Error");
    break;
 }
    
  delay(1000);
  lcd.clear();
}
