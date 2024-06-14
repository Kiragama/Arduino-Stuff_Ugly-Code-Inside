#ifndef Display_h
#define Display_h

///#include "DFRobot_GDL.h"
#include "ResourceSetup.h"
#include "W_IR.h"

#include <SPI.h>
#include <Wire.h>
//#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels

#define OLED_DC 7
#define OLED_CS 0
#define OLED_RESET 5



class Display_Stuff : /*public DFRobot_ST7735_128x160_HW_SPI,*/ public Adafruit_SSD1306, public Resources_Base {
protected:
  const int8_t textSize = 0; //default text size
  const int16_t textColor = SSD1306_WHITE;

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


public:
  Display_Stuff(): Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, OLED_DC, OLED_RESET, OLED_CS){ /*Anything in here doesn't do anything*/ };

  int8_t getTextSize() {
    return textSize;
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

  int16_t setAmmo();
  void initializeMenu();
  void menu();
};


#endif