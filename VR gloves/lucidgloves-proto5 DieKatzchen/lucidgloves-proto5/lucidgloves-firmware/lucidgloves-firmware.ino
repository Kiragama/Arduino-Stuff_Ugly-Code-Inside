#include "lucidgloves-firmware.h"
    
#define SIN PIN_PINKY
#define COS PIN_PINKY_SECOND
#define SPLAY PIN_PINKY_SPLAY
   
void setup() {
  Serial.begin(115200);  
}

void loop() {
  int sin = analogRead(SIN);
  int cos = analogRead(COS);
  int splay = analogRead(SPLAY);
  
  Serial.print("sin = ");
  Serial.print(sin);
  Serial.print(" cos = ");
  Serial.print(cos);
  Serial.print(" splay = ");
  Serial.println(splay);
  
  delay(100);
}
