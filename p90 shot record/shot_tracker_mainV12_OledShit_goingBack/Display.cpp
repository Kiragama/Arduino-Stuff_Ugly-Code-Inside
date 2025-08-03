#include "FreeSansBold24pt7b.h"

#include "Display.h"
#include "W_IR.h"
#include "Use_EEPROM.h"

W_IR disIR;
Use_EEPROM menee;


void Display_Stuff::loadTextNum(int8_t x, int8_t y, String lastVal, String newVal) {
  setCursor(x, y);  //Point is bottom left

  setTextColor(BACKCOLOUR);
  print(lastVal);
  setCursor(x, y);

  setTextColor(TEXTCOLOUR);
  print(newVal);  //can read but not write to ammo
}

void Display_Stuff::loadTextNum(int8_t x, int8_t y, int lastVal, int newVal) {
  setCursor(x, y);  //Point is bottom left

  setTextColor(BACKCOLOUR);
  print(lastVal);
  setCursor(x, y);

  setTextColor(TEXTCOLOUR);
  print(newVal);  //can read but not write to ammo
}

void Display_Stuff::printReload() {
  fillScreen(BACKCOLOUR);
  setCursor(30, 140);
  setTextSize(1);
  print(F("Reload"));  //signal to reload
  setTextSize(TEXTSIZE);
}

void Display_Stuff::printText(String text) {
  fillScreen(BACKCOLOUR);
  setCursor(30, 140);
  setTextSize(1);
  print(text);  //print value
  setTextSize(TEXTSIZE);
}


void Display_Stuff::printAmount() {  //print current mag count
  //check reload
  if (getammo() <= 0) {
    printReload();
  } else {
    setFont(&FreeSansBold24pt7b);
    setTextSize(TEXTSIZE);
    loadTextNum(30, 120, getlastammo(), getammo());  //prints text and overwrites last text
    setlastammo(getammo());
  }
}



void Display_Stuff::clearInput() {  //if set to NULL then would be treated as 0
  setkeyPressed(9001);
}


int16_t Display_Stuff::setAmmo() {  //sets ammo
  bool clrBtn = true;
  delay(250);
  while (clrBtn)  //this is so if the clear button pressed, if not cleared then this should never loop
  {
    int16_t sizeInput = 2;
    int16_t input[sizeInput];
    int16_t mag = 0;
    int16_t count = 0;
    fillScreen(BACKCOLOUR);  //clear
    setTextColor(TEXTCOLOUR);
    setFont(&FreeSans12pt7b);
    setCursor(x, 30);


    print("Enter mag size");
    setCursor(x, height() - 40);  //50 from the bottom
    drawFastHLine(x, height() - 25, width() - 70, TEXTCOLOUR);


    while (clrBtn) {
      setkeyPressed(disIR.irMyController());  //can't just set keypressed in the function. Not sure why but it doesn't feed back to the other class
                                              //needed to reduce double presses
      //Serial.println(getkeyPressed());
      if (getkeyPressed() <= 9 && count < sizeInput) {  //character limit Must be 2
        input[count] = getkeyPressed();                 //if the values
        setTextSize(1);
        print(input[count]);
        count++;

      } else if (getkeyPressed() == 16)  //if back pressed
      {
        clrBtn = false;  //stops both loops so it can go back to main menu. Count set to 0 so things skipped
        count = 0;
        fillScreen(BACKCOLOUR);
      } else if (getkeyPressed() == 10)  //if clear pressed
      {
        count = 0;
        fillScreen(BACKCOLOUR);
        break;

        //clear btn. Let it loop through so the while loop starts again
      } else if (getkeyPressed() == 18 && count == sizeInput)  //confirm. If input array full
      {
        clrBtn = false;  //break both loops
      }
    }


    if (count != 0 && clrBtn == false)  //if the above section was skipped then count will still be 0. If confirm pressed, then count would not be
    {
      unsigned pow = 10;
      while (input[sizeInput - 1] >= pow)
        pow *= 10;
      mag = input[0] * pow + input[sizeInput - 1];
      //only works with a size of 2

      //Confirm Section
      fillScreen(BACKCOLOUR);
      setCursor(5, 20);
      setTextSize(1);

      print("mag is now ");
      print(mag);

      delay(2000);                 //leave it on screen
      fillScreen(BACKCOLOUR);  //clear screen
      return mag;
    }
  }
  return 0;
}


void Display_Stuff::initializeMenu() {  //create the menu list
  fillScreen(BACKCOLOUR);
  setTextColor(TEXTCOLOUR);
  setTextSize(1);
  setFont(&FreeSans9pt7b);
  setCursor(0, 0);
  int lastVal = 20;
  size_t size = sizeof(menuItems) / sizeof(menuItems[0]);  //gets size of array remote
  for (int16_t i = 0; i < size; i++) {
    lastVal += 20;
    setCursor(15, lastVal);  //screen starts at the bottom left and because rotated, we need to print in the reverse order and from the top. But because the screen is rotated, the top for us is the max value of width
    print(menuItems[i]);
  }
}


void Display_Stuff::menu() {
  bool breakL = true;
  initializeMenu();  //display the menu items

  while (breakL) {
    setkeyPressed(disIR.irMyController());  //run IR first
    switch (getkeyPressed()) {
      case 0:  //break loop and menu
        breakL = false;
        fillScreen(BACKCOLOUR);
        break;
      case 1:
        setMag(setAmmo());  //sets resourcesetup mag to returned value of setAmmo function
        breakL = false;
        setammo(getMag());       //resets ammo count
        menee.saveEE(getMag());  //saves to EEPROM
        break;
      case 2:
        //save
        menee.saveEE(getMag());
        break;
      case 3:
        //load
        setMag(menee.loadEE());
        setammo(getMag());
        break;
      case 12:
        //reboot via power button
        menee.reboot();
        break;
    }
  }
}
