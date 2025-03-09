#ifndef Display_h
#define Display_h

#include "DFRobot_GDL.h"
#include "ResourceSetup.h"
#include "W_IR.h"

#define TFT_DC 1
#define TFT_CS 7 
#define TFT_RST 2 



class Display_Stuff : public DFRobot_ST7789_172x320_HW_SPI, public Resources_Base {
protected:
  const int8_t textSize = 3;
  const int16_t backColor = COLOR_RGB565_BLACK;
  const int16_t textColor = COLOR_RGB565_OLIVE;

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
  int16_t getBackColor() {
    return backColor;
  }
  int16_t getTextColor() {
    return textColor;
  }
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


#endif