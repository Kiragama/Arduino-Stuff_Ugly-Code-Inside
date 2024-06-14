/*!
 * @file basicTest.ino
 * @brief Demonstrate various graphic painting effects
 * @n This demo supports Arduino Uno, Leonardo, Mega2560, FireBeetle-ESP32, FireBeetle-ESP8266, and FireBeetle-M0.
 * @copyright Copyright (c) 2010 DFRobot Co. Ltd (http://www.dfrobot.com)
 * @license The MIT License (MIT)
 * @author [fary] (feng.yang@dfrobot.com)
 * @version V1.0
 * @date 2021-11-04
 * @url https://github.com/DFRobot/DFRobot_GDL
 */
#include "DFRobot_GDL.h"

#define TFT_DC 1
#define TFT_CS 7
#define TFT_RST 2

class test : public DFRobot_ST7735_128x160_HW_SPI
{
public:
test() : DFRobot_ST7735_128x160_HW_SPI(/*dc=*/TFT_DC, /*cs=*/TFT_CS, /*rst=*/TFT_RST) { };

};

test screen;
/*
byte red = 29;   // Red is the top 5 bits of a 16 bit colour value
byte green = 0;  // Green is the middle 6 bits
byte blue = 0;   // Blue is the bottom 5 bits
byte state = 0;*/
/*
 *User-selectable macro definition color
 *COLOR_RGB565_BLACK   COLOR_RGB565_NAVY    COLOR_RGB565_DGREEN   COLOR_RGB565_DCYAN 
 *COLOR_RGB565_MAROON  COLOR_RGB565_PURPLE  COLOR_RGB565_OLIVE    COLOR_RGB565_LGRAY     
 *COLOR_RGB565_DGRAY   COLOR_RGB565_BLUE    COLOR_RGB565_GREEN    COLOR_RGB565_CYAN  
 *COLOR_RGB565_RED     COLOR_RGB565_MAGENTA COLOR_RGB565_YELLOW   COLOR_RGB565_ORANGE           
 *COLOR_RGB565_WHITE   
 */
boolean initial = 1;
void setup() {
  Serial.begin(115200);
  screen.begin();
  screen.fillScreen(COLOR_RGB565_DCYAN);
  screen.setRotation(3);
  screen.print("stuff");
}
int count = 0;

void loop() {
  screen.setTextSize(1);
  /* Set screen color */
  /*
   *The currently available fonts are as follows, you can add other font files in gfxfont.h (font files are stored in the src/Frame/Fonts folder)
   *FreeMono9pt7b, FreeMono12pt7b, FreeMonoBold12pt7b,
   *FreeMonoBoldOblique12pt7b, FreeMonoOblique12pt7b,
   *FreeSans12pt7b,FreeSansBold12pt7b, FreeSansBoldOblique12pt7b,
   *FreeSansOblique12pt7b, FreeSerif12pt7b, FreeSerifBold12pt7b,
   *FreeSerifBoldItalic12pt7b, FreeSerifItalic12pt7b, FreeMono24pt7b
   */
  screen.setFont(&FreeSans9pt7b);  //Set the font to FreeMono12pt7b

  /*
   * @ brief Set text position
   * @ param x The x-coordinate of the first word of the text
   * @ param y The y-coordinate of the first word of the text 
   */
  screen.setCursor(/*x=*/10, /*y=*/screen.height() - 108);
  // Set the text color
  // The optional color list is the same as the color list used in the fillScreen function
  screen.setTextColor(COLOR_RGB565_BLACK);
  //Set to text auto-wrapping mode
  //true = Auto-wrap, false =No auto-warp
  screen.setTextWrap(true);
  //Output text
  while (1) {
    screen.setTextColor(COLOR_RGB565_BLACK);
    screen.print(count);
    delay(500);
    screen.setCursor(/*x=*/10, /*y=*/screen.height() - 108);
    screen.setTextColor(COLOR_RGB565_DGRAY); //background colour
    screen.print(count);
    count++;  //prints count,
  }
}