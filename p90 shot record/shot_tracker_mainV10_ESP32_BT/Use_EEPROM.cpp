

#include "Use_EEPROM.h"





void Use_EEPROM::saveEE(float mag) {  //save magazine to an address in EEPROM
  //eeAddress += sizeof(float); //Move address to the next byte after previous entry.
  EEPROM.writeFloat(eeAddress, mag);  //data must be a float
  EEPROM.commit();                    //save

  Serial.print("saved EEPROM");
}

////////////////////////////////////////////////////////////////////////////////////////

float Use_EEPROM::loadEE() {  //saved values into magazine
  float i = EEPROM.readFloat(eeAddress);

  Serial.print("Loaded EEPROM");
  return i;
}

////////////////////////////////////////////////////////////////////////////////////////

void Use_EEPROM::reboot() {  //reboots device
  ESP.restart();
}