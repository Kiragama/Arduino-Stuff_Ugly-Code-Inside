#ifndef Use_EEPROM_h
#define Use_EEPROM_h


#include <EEPROM.h>

#include "ResourceSetup.h"


class Use_EEPROM : public Resources_Base {
public:
  Use_EEPROM() { }
  void saveEE(float mag); //eeprom not saving seems to be esp32 issue
  float loadEE(); //might be same for loads
  void reboot();

private:
  const int16_t eeAddress = 0;  //address of mag
};



#endif