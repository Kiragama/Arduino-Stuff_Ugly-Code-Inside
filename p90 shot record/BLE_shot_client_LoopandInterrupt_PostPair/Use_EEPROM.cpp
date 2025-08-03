#include "Use_EEPROM.h"

void Use_EEPROM::reboot() {  //reboots device
  Serial.println("Restarting");
  Serial.flush();
  Serial.end();
  ESP.restart();
}

float Use_EEPROM::loadDis(int address) {
  float i = EEPROM.readFloat(address);
  Serial.print("Loaded EEPROM. Loaded: ");
  Serial.println(i);
  return i;
}

void Use_EEPROM::saveDis(float dis, int address) {  //save magazine to an address in EEPROM
  //eeAddress += sizeof(float); //Move address to the next byte after previous entry.
  EEPROM.writeFloat(address, dis);  //data must be a float
  EEPROM.commit();                         //save

  Serial.print("saved EEPROM, which is: ");
  Serial.println(dis);
}