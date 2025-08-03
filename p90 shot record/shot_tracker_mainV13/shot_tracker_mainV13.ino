/*

Author: Kiragama

Notes:
To pair, Press pair on display, wait 5 seconds, press pair on sensor.

There is no delay or main reason for this other then to make it more reliable. If not paired after 30+ seconds. Press pair on sensor again.
Power cycling will not remove pairMode.

Pair will reset mag count to solve a bug with tickdown running forever.
###################################################################################
*/



#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <math.h>
#include <Arduino.h>
#include <string.h>

#include "Defines.h"
#include "Use_EEPROM.h"
#include "Display.h"
#include "ResourceSetup.h"
#include "W_IR.h"

unsigned long last_time;

Display_Stuff inoDisplay;
Use_EEPROM inoee;
W_IR irUse;

BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
BLEServerCallbacks* pServerCallbacks = NULL;
bool deviceConnected = false;
bool once = true;
bool isInPairMode = false;
bool restartedInPairMode = false;
bool resetMag = false;

#include "InterruptsFile.h"
#include "Classes_and_Functions.h"

void setup() {
  Serial.begin(9600);

  Serial.println("setup start");
  EEPROM.begin(EEPROM_SIZE);

  pinMode(RELOADPIN, INPUT_PULLUP);
  pinMode(RELOADRF, INPUT);
  pinMode(PAIRBTN, INPUT_PULLUP);
  //gpio_deep_sleep_hold_en();  //hold the pinmode during sleep

  inoDisplay.begin();
  inoDisplay.setRotation(1);  //flips 180" (0=0, 1=90, 2=180, 3=270)

  last_time = millis();


  //SET AMMO
  Serial.println("will load ee");
  float tmp = inoee.loadEE(EEADDRESS);

  if (tmp > 0)  //if boots and magazine empty. Ask to set.
  {
    inoDisplay.setMag(tmp);
  } else {
    inoDisplay.setMag(inoDisplay.setAmmo());
    inoee.saveEE(inoDisplay.getMag(), EEADDRESS);
  }
  Serial.println(inoDisplay.getMag());
  inoDisplay.setammo(inoDisplay.getMag());  //sets ammo variable to the temp of magazine
  inoDisplay.fillScreen(BACKCOLOUR);
  //create first screen
  inoDisplay.printAmount();


  //INTERRUPTS

  
  attachInterrupt(digitalPinToInterrupt(RELOADRF), reloadRF, RISING);
  attachInterrupt(digitalPinToInterrupt(RELOADPIN), reload, FALLING);
  attachInterrupt(digitalPinToInterrupt(PAIRBTN), pair, FALLING);
  Serial.println("post interrupt setup");
  //BLE SETUP
  Serial.println("Starting BLE work!");
  //setup of server and creation of the BLE service on above UUID


  //LOAD NAME
  String disName;
  float tmpDisplayIn = inoee.loadDis(NAMEADDRESS);
  if (tmpDisplayIn >= 100) {                                        //1.23 is used to determin if the Shot tracker Display has been reset to default. Any actual value will be over 100 so, this checks that.
    disName = "Shot_Tracker_Display_Tag-" + String(int(tmpDisplayIn));  //Sets new device name to whatever was recieved from the sensor.
    Serial.print("Loaded Name, which is: ");                        //debug
    Serial.println(disName);
    inoDisplay.setMag(tmp);  //To reset mag
  } else {
    disName = "Shot tracker Display";  //resets to default so a sensor can connect and send their pair value.
    restartedInPairMode = true;        //as it reboots, pairmode will need setting again
    Serial.println("Rebooted in Pairmode");
    Serial.println("Default Name");
    inoDisplay.printText("Pair mode", TEXTSIZE);
  }

  BLEDevice::init(disName);  //device name (char limit)
  pServer = BLEDevice::createServer();
  BLEService* pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);

  pCharacteristic->setValue("Neil says Hi");
  pService->start();
  BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("Characteristic defined! Can be detected");

  //set recieved calls/values to corresponding classes. Only done as 2 classes as would not work as one class.
  pServer->setCallbacks(new MyServerCallback());
  pCharacteristic->setCallbacks(new MyCallbacks());
  //END OF BLE SETUP
}

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
  if (!deviceConnected && once && !restartedInPairMode) { //while not connected and restarted in pairmode, only display the initial pairmode
    inoDisplay.printText("Disconnected", TEXTSIZE);  //displays disconnected once and stays on screen till connected
    once = false;
  } else if (deviceConnected && !once) {  //displays connect and not again until disconnected and connected
    inoDisplay.printText("Connected", TEXTSIZE);
    once = true;
    delay(3000);                        //delay to ensure a proper connection and stop back and forths during connection
    inoDisplay.fillScreen(BACKCOLOUR);  //blacks screen to clear text. print amount only replaces the text area
    inoDisplay.printAmount();
  }

  inoDisplay.refreshCounter++;
  if (inoDisplay.refreshCounter >= 2000 && deviceConnected) {  //display needs to update every once in a while for the battery drain to update but always updating is an issue
    inoDisplay.printAmount();
  }


  if (isInPairMode) {                  //this is a check for if the interupt has  been triggered. This does not happen in interrupt as writing to EEPROM would crash esp
    inoee.saveDis(1.23, NAMEADDRESS);  //1.23 used to flag the default
    inoDisplay.printText("Rebooting", TEXTSIZE);
    delay(3000);                        //for debug and can actually see the print
    inoee.reboot();                    //reboots so on setup, the name is default. The sensor will connect and then send the pair value.
  }

  if (resetMag && !restartedInPairMode) { //resets ammo to keep what the interrupt does down and does not function in pairmode. (also setammo crashes if in interrupt)
    resetMag = false;
    inoDisplay.setammo(inoDisplay.getMag());
    inoDisplay.fillScreen(BACKCOLOUR);
    inoDisplay.printAmount();
  }
}
