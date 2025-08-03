#pragma once

#include "DFRobot_GDL.h"
#include "ResourceSetup.h"
#include "W_IR.h"
#include "Defines.h"


class Display_Stuff : public DFRobot_ST7789_172x320_HW_SPI, public Resources_Base {
protected:
  const String menuItems[4]{
    "0 Exit",
    "1 Set Ammo",
    "2 Save EEPROM",
    "3 Load EEPROM",
  };
  int16_t x = 20;

public:
  Display_Stuff()
    : DFRobot_ST7789_172x320_HW_SPI(/*dc=*/TFT_DC, TFT_CS, TFT_RST){ /*Anything in here doesn't do anything*/ };
  ~Display_Stuff() { };

  void createRec();
  void printReload();
  void printText(String text);
  void printAmount();
  void clearInput();
  int16_t displayHeight() {
    return Display_Stuff::height();
  };
 

  int16_t setAmmo();
  void initializeMenu();
  void menu();

  //Overloaded Functions
  //prints text and overwrites last text
  void loadTextNum(int8_t x, int8_t y, String lastVal, String newVal); 
  //prints text and overwrites last text
  void loadTextNum(int8_t x, int8_t y, int lastVal, int newVal);
};
