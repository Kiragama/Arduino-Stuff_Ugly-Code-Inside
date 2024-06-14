#ifndef Display_h
#define Display_h

#include "DFRobot_GDL.h"
#include "ResourceSetup.h"
#include "W_IR.h"

#define TFT_DC 1
#define TFT_CS 7
#define TFT_RST 2
#define TFT_SD  9


class Display_Stuff : public DFRobot_ST7735_128x160_HW_SPI, public Resources_Base {
protected:
  const int8_t textSize = 2;
  const int16_t backColor = COLOR_RGB565_BLACK;
  const int16_t textColor = COLOR_RGB565_MAGENTA;

  const int batColor = 0x07E0;          //green in RGB565
  const float batMaxVolt = 4.2;         //9.80 is the max voltage of the NiMF bat
  const int16_t BAT_TOTAL_HEIGHT = 16;  //the box must be 16bytes long and basically just pasting the 1s onto the screen.
  const int16_t BAT_TOTAL_WIDTH = 16;
  const int8_t BAT_WOFFSET = 5;  //offset of the batter itself from the 16 bit screen do -1 for the bottom
  const int8_t BAT_X = 10;       //battery x position
  const float minBat = 3.5;
  const int8_t BAT_WIDTH = 8;    //width of internal volume
  const int8_t BAT_HEIGHT = 10;  //height of internal volume
  int16_t BAT_Y = 80;            //the Y position of the top of the full battery image
  const String menuItems[4]{
    "0 Exit",
    "1 Set Ammo",
    "2 Save EEPROM",
    "3 Load EEPROM",
  };
  int16_t x = 20;

public:
  Display_Stuff()
    : DFRobot_ST7735_128x160_HW_SPI(/*dc=*/TFT_DC, /*cs=*/TFT_CS, /*rst=*/TFT_RST){ /*Anything in here doesn't do anything*/ };

  int16_t getBackColor() {
    return backColor;
  }
  int16_t getTextColor() {
    return textColor;
  }
  void createRec();
  void printReload();
  void printAmount();
  void clearInput();
  int16_t displayHeight() {
    return Display_Stuff::height();
  };
  void createBat();
  void fillBat();
  void drainBat();
  float getVoltage();
  const int16_t readVoltage = 50;
  const unsigned char PROGMEM batLogo[32] = { 0b00000111, 0b11100000,
                                              0b00000111, 0b11100000,
                                              0b11111111, 0b11111111,
                                              0b10000000, 0b00000001,
                                              0b10000000, 0b00000001,
                                              0b10000000, 0b00000001,
                                              0b10000000, 0b00000001,
                                              0b10000000, 0b00000001,
                                              0b10000000, 0b00000001,
                                              0b10000000, 0b00000001,
                                              0b10000000, 0b00000001,
                                              0b10000000, 0b00000001,
                                              0b10000000, 0b00000001,
                                              0b10000000, 0b00000001,
                                              0b10000000, 0b00000001,
                                              0b11111111, 0b11111111 };

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