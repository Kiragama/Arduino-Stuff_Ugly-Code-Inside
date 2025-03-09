#include "FreeSansBold24pt7b.h"


#include "Display.h"
#include "W_IR.h"
#include "Use_EEPROM.h"

W_IR disIR;
Use_EEPROM menee;




////////////////////////////////////////////////////////////////////////////////////////
void Display_Stuff::loadTextNum(int8_t x, int8_t y, String lastVal, String newVal) {
  setCursor(x, y);  //Point is bottom left

  setTextColor(backColor);
  print(lastVal);
  setCursor(x, y);

  setTextColor(textColor);
  print(newVal);  //can read but not write to ammo
}
////////////////////////////////////////////////////////////////////////////////////////
void Display_Stuff::loadTextNum(int8_t x, int8_t y, int lastVal, int newVal) {
  setCursor(x, y);  //Point is bottom left

  setTextColor(backColor);
  print(lastVal);
  setCursor(x, y);

  setTextColor(textColor);
  print(newVal);  //can read but not write to ammo
}
////////////////////////////////////////////////////////////////////////////////////////
void Display_Stuff::printReload() {
  fillScreen(backColor);
  setCursor(15, 40);
  print(F("Reload"));  //signal to reload
  /*if ((getVoltage() > 0)) {

    createBat();
    //drainBat();  //Drain works, just not when no battery connected

  } else {

    setCursor(BAT_X, BAT_Y);
    setFont(&FreeSansBold9pt7b);
    setTextColor(COLOR_RGB565_RED);
    print("X");
  }*/
}
////////////////////////////////////////////////////////////////////////////////////////
void Display_Stuff::printAmount() {  //print current mag count
  //check reload
  if (getammo() <= 0) {
    printReload();
  } else {
    //setFont(&FreeSansBold24pt7b); //changed
    setTextSize(textSize);
    setCursor(10, 10);
    print(".");
    loadTextNum(70, 40, getlastammo(), getammo());  //prints text and overwrites last text //changed 30, 80
    setlastammo(getammo());
/*
    if ((getVoltage() > 0)) {

      //createBat();
      //drainBat();  //Drain works, just not when no battery connected

    } else {

      setCursor(BAT_X, BAT_Y);
      setFont(&FreeSansBold9pt7b);
      setTextColor(COLOR_RGB565_RED);
      print("X");
    }*/
  }
}
////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////
void Display_Stuff::clearInput() {  //if set to NULL then would be treated as 0
  setkeyPressed(9001);
}
/*  Reading voltage on arduino/ESP32 requires a voltage devider that devides by 5. This allows the device to read that value and calculate the voltage. The ADC on arduino and ESP32 C3 is limited to around 1.1v. 
//For this version, I am going to forgo the battery monitor so it is actually completed
//_______BATTERY!!_________________________________________
void Display_Stuff::createBat() {                                                           //create and display the empty battery. Call fillBat
  drawXBitmap(BAT_X - 4, BAT_Y + 9, batLogo, BAT_TOTAL_WIDTH, BAT_TOTAL_HEIGHT, batColor);  //challise
  fillBat();
}
////////////////////////////////////////////////////////////////////////////////////////
void Display_Stuff::fillBat() {                                       //fills and displays the full battery
  fillRect(BAT_X, BAT_Y, BAT_WIDTH, BAT_HEIGHT, COLOR_RGB565_WHITE);  //fill the battery internals box. Used because it distinguishes the empty and full batteries without using 2 bitmaps
  fillRect(BAT_X + 2, BAT_Y - 3, 4, 8, COLOR_RGB565_WHITE);
}
////////////////////////////////////////////////////////////////////////////////////////
void Display_Stuff::drainBat() {
  float volt = (getVoltage() - minBat) / (batMaxVolt - minBat) * 100;  //calculates the battery percentage and takes into consideration the lowest the battery can go
  float thresh = 100 / BAT_HEIGHT + 3;                                 //BAT_HEIGHT is 9 and this gets the number of 9's in 100. Its 11.11
  float invert = 100 - volt;                                           //takes voltage percentage off 100, inverting it.
  float pipNum = invert / thresh;
  //how many theshholds are in the inverted value. e.g. if invert is 20 and thresh is 11, then this is how many 11s in 20.
  //As invert is created from missing percentage and thresh is how many 9s in 100, then this should return how many battery pips have been used. This can then be taken off the BAT_HEIGHT
  int32_t final_val = round(pipNum);  //rounds to the nearest whole number as is safer when the battery is low (gives more time by being slightly wrong)

  fillRect(BAT_X, BAT_Y - 3, BAT_WIDTH, final_val, COLOR_RGB565_BLACK);  //creates a box at the top of bat and this is made of inverse colour. This gets bigger as it drains and grows downwards. should end at BAT_HEIGHT
  //this is accurate but remember that the voltage read isn't always accurate. ATM it is 0.5v lower than real
}
////////////////////////////////////////////////////////////////////////////////////////
float Display_Stuff::getVoltage() {  //gets battery voltage
  //this assumes the input voltage is being converted from a higher voltage to 5v. if input is already <=5v then remove section involving the result variable
  // keep an eye out for noise on that pin (changing values)
  //if noise is minimal and the input set correctly, this should be accurate to the 2nd decimal point.

  float inputRef = 5;  //5.02this varies from device to device. on ATMEGA32U4, measure voltage of pin 24 (AVCC) on the chip and use that here.
  float in = (float)analogRead(readVoltage);
  float voltage = in * inputRef / 1023.0;  //1023 is the steps value for a 10 bit A/D converter. inputRef is the max voltage for the AVCC pin on the ATMEGA32U4, I just measured the out voltage of pin24 on the chip. These 2 are what makes the voltage accurate as some chips might not have a 10bit A/D converter or have 4.25 as max
  //source https://www.youtube.com/watch?v=gw72g4WBz-U
  //power supply can be noisy and will affect accuracy of this reader as the basis this reader uses to measure is that input voltage (measured from AVCC by default)

  //to restore the original voltage before conversion
  //float result = voltage * 5;  //the times value is the input voltage / output of conversion


  if (result <= minBat && result > 0)  //if min voltage reached
  {
    fillScreen(backColor);
    setTextSize(0);
    setFont(&FreeSansBold9pt7b);
    setCursor(10, 30);
    print("Please Turn Off");
    while (true) {}  //never end and force it to be powered off
  }
  return voltage;
}//_______BATTERY!!_________________________________________

*/
////////////////////////////////////////////////////////////////////////////////////////
int16_t Display_Stuff::setAmmo() {  //sets ammo
  bool clrBtn = true;
  delay(250);
  while (clrBtn)  //this is so if the clear button pressed, if not cleared then this should never loop
  {
    int16_t sizeInput = 2;
    int16_t input[sizeInput];
    int16_t mag = 0;
    int16_t count = 0;
    fillScreen(getBackColor());  //clear
    setTextColor(getTextColor());
    setFont(&FreeSans9pt7b);
    setCursor(x, 20);


    print("Enter mag size");
    setCursor(x, height() - 40);  //50 from the bottom
    drawFastHLine(x, height() - 25, width() - 70, COLOR_RGB565_WHITE);


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
        fillScreen(getBackColor());
      } else if (getkeyPressed() == 10)  //if clear pressed
      {
        count = 0;
        fillScreen(getBackColor());
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
      fillScreen(getBackColor());
      setCursor(5, 20);
      setTextSize(1);

      print("mag is now ");
      print(mag);

      delay(2000);                 //leave it on screen
      fillScreen(getBackColor());  //clear screen
      return mag;
    }
  }
  return 0;
}

////////////////////////////////////////////////////////////////////////////////////////
void Display_Stuff::initializeMenu() {  //create the menu list
  fillScreen(getBackColor());
  setTextColor(getTextColor());
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

////////////////////////////////////////////////////////////////////////////////////////

void Display_Stuff::menu() {
  bool breakL = true;
  initializeMenu();  //display the menu items

  while (breakL) {
    setkeyPressed(disIR.irMyController());  //run IR first
    switch (getkeyPressed()) {
      case 0:  //break loop and menu
        breakL = false;
        fillScreen(getBackColor());
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