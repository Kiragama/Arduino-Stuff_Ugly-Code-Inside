#include "FreeSansBold24pt7b.h"
#include "/home/william/Arduino/libraries/Adafruit_GFX_Library/Fonts/FreeSans9pt7b.h"

#include "Display.h"
#include "W_IR.h"
#include "Use_EEPROM.h"

W_IR disIR;
Use_EEPROM menee;


////////////////////////////////////////////////////////////////////////////////////////
void Display_Stuff::printReload() {
  clearDisplay();
  setFont(NULL); //reset font as not used here (size issues)
  setCursor(10, 10);
  print(F("Reload"));  //signal to reload
  display();
}
////////////////////////////////////////////////////////////////////////////////////////
void Display_Stuff::printAmount() {  //print current mag count
  //check reload
  if (getammo() <= 0) {
    printReload();
  } else {
    setFont(&FreeSansBold24pt7b);
    clearDisplay();
    setTextColor(getTextColor());
    setTextSize(0);
    setCursor(70,46);
    print(getammo());
    Serial.println(" printed");
    Serial.println(getammo());
    display();
  }
}
////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////
void Display_Stuff::clearInput() {  //if set to NULL then would be treated as 0
  setkeyPressed(9001);
}
////////////////////////////////////////////////////////////////////////////////////////
int16_t Display_Stuff::setAmmo() {  //sets ammo
  bool clrBtn = true;
  delay(250);
  int8_t x = 5;
  while (clrBtn)  //this is so if the clear button pressed, if not cleared then this should never loop
  {
    int16_t sizeInput = 2;
    int16_t input[sizeInput];
    int16_t mag = 0;
    int16_t count = 0;
    clearDisplay();
    setTextColor(getTextColor());
    setFont(&FreeSans9pt7b);
    setCursor(x, 20);


    print("Enter mag size");
    setCursor(x, height() - 27);  //50 from the bottom
    drawFastHLine(x, height() - 25, width() - 70, getTextColor());
    display();

    while (clrBtn) {
      setkeyPressed(disIR.irMyController());  //can't just set keypressed in the function. Not sure why but it doesn't feed back to the other class
                                              //needed to reduce double presses
      Serial.println(getkeyPressed());
      if (getkeyPressed() <= 9 && count < sizeInput) {  //character limit Must be 2
        input[count] = getkeyPressed();                 //if the values
        setTextSize(getTextSize());
        print(input[count]);
        display();
        count++;

      } else if (getkeyPressed() == 16)  //if back pressed
      {
        clrBtn = false;  //stops both loops so it can go back to main menu. Count set to 0 so things skipped
        count = 0;
        clearDisplay();
      } else if (getkeyPressed() == 10)  //if clear pressed
      {
        count = 0;
        clearDisplay();
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
      clearDisplay();
      setCursor(5, 20);
      setTextSize(getTextSize());

      print("mag is now ");
      print(mag);
      display();

      delay(2000);                 //leave it on screen
      clearDisplay();  //clear screen
      return mag;
    }
  }
  return 0;
}

////////////////////////////////////////////////////////////////////////////////////////
void Display_Stuff::initializeMenu() {  //create the menu list
  clearDisplay();
  setTextColor(getTextColor());
  setTextSize(getTextSize());
  setCursor(0, 0);
  int lastVal = 10; //gap
  size_t size = sizeof(menuItems) / sizeof(menuItems[0]);  //gets size of array remote
  for (int16_t i = 0; i < size; i++) {
    setCursor(15, lastVal);
    lastVal += 10;
    print(menuItems[i]);
    display();
  }
}

////////////////////////////////////////////////////////////////////////////////////////

void Display_Stuff::menu() {
  bool breakL = true;
  setFont(NULL); //reset font as not used here (size issues)
  initializeMenu();  //display the menu items

  while (breakL) {
    setkeyPressed(disIR.irMyController());  //run IR first
    switch (getkeyPressed()) {
      case 0:  //break loop and menu
        breakL = false;
        clearDisplay();
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
////////////////////////////////////////////////////////////////////////////////////////_____