#pragma once


#include <EEPROM.h>

#include "ResourceSetup.h"


class Use_EEPROM : public Resources_Base {
public:
  Use_EEPROM() { }
  void saveEE(float mag, int address); //eeprom not saving seems to be esp32 issue
  float loadEE(int address); //might be same for loads
  float loadDis(int address);
  void saveDis(float dis, int address);
  void reboot();

};


