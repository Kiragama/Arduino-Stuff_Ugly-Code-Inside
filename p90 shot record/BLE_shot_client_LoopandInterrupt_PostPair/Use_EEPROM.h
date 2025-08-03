#pragma once


#include <EEPROM.h>


class Use_EEPROM{
public:
  Use_EEPROM() { }
  float loadDis(int address);
  void saveDis(float dis, int address);
  void reboot();


};


