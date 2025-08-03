#include "Use_EEPROM.h"

void Use_EEPROM::saveEE(float mag, int address) {  //save magazine to an address in EEPROM
  //eeAddress += sizeof(float); //Move address to the next byte after previous entry.
  EEPROM.writeFloat(address, mag);  //data must be a float
  EEPROM.commit();                    //save

  Serial.print("saved EEPROM");
}

////////////////////////////////////////////////////////////////////////////////////////

float Use_EEPROM::loadEE(int address) {  //saved values into magazine
  float i = EEPROM.readFloat(address);

  Serial.println("Loaded EEPROM");
  return i;
}

////////////////////////////////////////////////////////////////////////////////////////

void Use_EEPROM::reboot() {  //reboots device
  Serial.println("Restarting");
  Serial.flush();
  ESP.restart();
}

float Use_EEPROM::loadDis(int address) {
  float i = EEPROM.readFloat(address);

  Serial.print("Loaded Display EEPROM. Loaded: ");
  Serial.println(i);
  return i;

}

void Use_EEPROM::saveDis(float dis, int address) {  //save magazine to an address in EEPROM
  //eeAddress += sizeof(float); //Move address to the next byte after previous entry.
  EEPROM.writeFloat(address, dis);  //data must be a float
  EEPROM.commit();                    //save

  Serial.println("saved Display EEPROM");
}