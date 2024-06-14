#include <Arduino.h>
#include <string.h>
#include <math.h>

#define EEPROM_SIZE 10  //bytes


#include "Use_EEPROM.h"
#include "Display.h"
#include "ResourceSetup.h"
#include "W_IR.h"


const int16_t chkReload = 10;  //records reloads when this is HIGH
const int16_t readPin = 0;     //records shot when this is HIGH

unsigned long last_time;

Display_Stuff inoDisplay;
Use_EEPROM inoee;
W_IR irUse;

//_________________________________________

void IRAM_ATTR isr() {  //Pin change interrupt (no pin HIGH/LOW interrupt this fast on ATmega). Even though PCINT3 is being scanned. Only 0 will trigger. Any PCINTn will trigger this funtion. Can do do internal checks regarding this
  if ((micros() > last_time + 10)) {
    //not only will interupt be trigger but because of how fast this is, you can check if that same pin is HIGH/LOW (high in this case)
    if (inoDisplay.getammo() > 0)  //process for magazine counter. Simple process. Count and stop when mag capacity reached
    {
      inoDisplay.setlastammo(inoDisplay.getammo());
      inoDisplay.setammo(inoDisplay.getammo() - 1);
      inoDisplay.printAmount();
      last_time = micros();
      //Serial.println(inoDisplay.getammo());
    }

    if (inoDisplay.getammo() == 0) {
      inoDisplay.printReload();  //required as the lcd_1 var is managed by Display_Stuff
    }
  }
}

//_________________________________________

void IRAM_ATTR reload() {
  inoDisplay.fillScreen(inoDisplay.getBackColor());
  inoDisplay.setammo(inoDisplay.getMag());
  inoDisplay.printAmount();
  //Reload being an interrupt is good as I can reload whenever
}


//____________________________________

void setup() {
  EEPROM.begin(EEPROM_SIZE);
  Serial.begin(9600);
  Serial.println("setup start");
  pinMode(readPin, INPUT);
  
  pinMode(chkReload, INPUT_PULLUP);
  gpio_deep_sleep_hold_en();  //hold the pinmode during sleep

  inoDisplay.begin();
  inoDisplay.setRotation(3);  //flips 180" (0=0, 1=90, 2=180, 3=270)

  last_time = micros();
  //set ammo
  Serial.println("will load ee");
  inoDisplay.setMag(inoee.loadEE());

  if (inoDisplay.getMag() < 0 && !inoDisplay.getMag())  //if boots and magazine empty. Ask to set. < 0 is because it will load the memory address otherwise. Likely -2147483648
  {
    inoDisplay.setMag(inoDisplay.setAmmo());
    inoee.saveEE(inoDisplay.getMag());
  }
  inoDisplay.setammo(inoDisplay.getMag());  //sets ammo variable to the temp of magazine
  inoDisplay.fillScreen(inoDisplay.getBackColor());
  //create first screen
  inoDisplay.printAmount();
  //Serial.println(inoDisplay.getammo());
  //Serial.println(inoDisplay.getMag());

  //_________________________________________Interrupts
  attachInterrupt(digitalPinToInterrupt(readPin), isr, RISING);
  attachInterrupt(digitalPinToInterrupt(chkReload), reload, RISING);
  Serial.println("post interrupt setup");
  if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_GPIO) {
    isr();  //if woken from sleep, then call interrupt
  }

  esp_deep_sleep_enable_gpio_wakeup(GPIO_NUM_1, ESP_GPIO_WAKEUP_GPIO_HIGH);  //I am unsure but the pin 0 is GPIO 0 but GPIO 1 triggers. Interrupt

  //_________________________________________Interrupts
}

//_________________________________________

void loop() {
  
  
  inoDisplay.setkeyPressed(irUse.irMyController());  //run IR first
  switch (inoDisplay.getkeyPressed()) {
    case 13:  //menu
      inoDisplay.menu();

      inoDisplay.printAmount();
      break;
    case 12:  //Power
      inoee.reboot();
      break;
  };

  inoDisplay.refreshCounter++;
  if (inoDisplay.refreshCounter >= 2000) {  //display needs to update every once in a while for the battery drain to update but always updating is an issue
    inoDisplay.printAmount();
    inoDisplay.drainBat();
  }
}
