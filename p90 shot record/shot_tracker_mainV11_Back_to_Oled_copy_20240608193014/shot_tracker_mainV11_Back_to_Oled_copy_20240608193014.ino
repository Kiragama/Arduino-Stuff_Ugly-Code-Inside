#include <Arduino.h>
#include <string.h>
#include <math.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#define EEPROM_SIZE 10  //bytes


#include "Use_EEPROM.h"
#include "Display.h"
#include "ResourceSetup.h"
#include "W_IR.h"

const int16_t chkReload = 10;  //records reloads when this is HIGH

unsigned long last_time;

Display_Stuff inoDisplay;
Use_EEPROM inoee;
W_IR irUse;

////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////START OF BLE5////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
// See the following for generating UUIDs:
// https://www.uuidgenerator.net/
#define SERVICE_UUID "6bc38e09-5e61-4838-8d1b-f3e093a2c5e6"  //server advertises and as long as client has the same UUID and Characteristic UUID. Plenty of security holes here
#define CHARACTERISTIC_UUID "b3286334-8358-4b98-9111-2b3cef9758c9"
BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
BLEServerCallbacks* pServerCallbacks = NULL;
bool deviceConnected = false;


//--Classes
class MyCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* pCharacteristic) {
    std::string value = pCharacteristic->getValue();  //Allways gets value as string so better sending as one aswell


    if (value.length() > 0) {
      float count;
      char tmp[value.length()];
      Serial.print("New value: ");
      for (int i = 0; i < value.length(); i++) {
        tmp[i] = value[i];
      }

      count += atol(tmp);
      inoDisplay.setammo(inoDisplay.getammo() - count); //reduce ammo by the value of count. 
      //Remember, it transmits in batches and not per shot.
      inoDisplay.printAmount();
      Serial.print(count);
      Serial.println();
    }
  }
};

class MyServerCallback : public BLEServerCallbacks {
  void onConnect(BLEServer* pserver) {
    Serial.println("onConnect");
    deviceConnected = true;
  }

  void onDisconnect(BLEServer* pserver) {

    Serial.println("onDisconnect");
    deviceConnected = false;
    delay(500);                   // give the bluetooth stack the chance to get things ready
    pServer->startAdvertising();  // restart advertising
    Serial.println("start advertising");
  }
};
//--Classes End


////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////// END OF BLE////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////

void IRAM_ATTR reload() {
  inoDisplay.setammo(inoDisplay.getMag());
  inoDisplay.printAmount();
  Serial.print("Reloaded");
  //Reload being an interrupt is good as I can reload whenever
}

////////////////////////////////////////////////////////////////////////////////////////

void setup() {
  Serial.begin(9600);
  ////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////START OF BLE5////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////
  Serial.println("Starting BLE work!");
  //setup of server and creation of the BLE service on above UUID
  BLEDevice::init("Shot tracker Display");  //device name (char limit)
  pServer = BLEDevice::createServer();
  BLEService* pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);

  pCharacteristic->setValue("Hello World says Neil");
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
  ////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////// END OF BLE////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////

  ////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////// EEPROM and Display////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////

  EEPROM.begin(EEPROM_SIZE);
  Serial.println("setup start");
  pinMode(chkReload, INPUT_PULLDOWN);
  //gpio_deep_sleep_hold_en();  //hold the pinmode during sleep
  inoDisplay.begin();
  inoDisplay.setRotation(0);  //flips 180" (0=0, 1=90, 2=180, 3=270)
  inoDisplay.setTextColor(inoDisplay.getTextColor());

  last_time = micros();
  //set ammo
  Serial.println("will load ee");
  float tmp = inoee.loadEE();

  if (tmp > 0)  //if boots and magazine empty. Ask to set.
  {
    inoDisplay.setMag(tmp);
    Serial.print("Loaded EEPROM");
  } else {
    inoDisplay.setMag(inoDisplay.setAmmo());
    //inoDisplay.setMag(50); //for testing
    inoee.saveEE(inoDisplay.getMag());
    Serial.print("not loaded EEPROM");
  }
  Serial.print(inoDisplay.getMag());
  inoDisplay.setammo(inoDisplay.getMag());  //sets ammo variable to the temp of magazine
  //create first screen
  inoDisplay.printAmount();
  ////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////// EEPROM and Display////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////

  ////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////Interupts////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////
  attachInterrupt(digitalPinToInterrupt(chkReload), reload, CHANGE);
  Serial.println("post interrupt setup");

  //sleep not in use at the moment as battery life last long enough where it doesn't matter.
  /*if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_GPIO) {
    isr();  //if woken from sleep, then call interrupt
  }
  esp_deep_sleep_enable_gpio_wakeup(GPIO_NUM_1, ESP_GPIO_WAKEUP_GPIO_HIGH);  //I am unsure but the pin 0 is GPIO 0 but GPIO 1 triggers. Interrupt
*/
  ////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////Interupts////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////

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
  if (inoDisplay.refreshCounter >= 200000) {  //display needs to update every once in a while for the battery drain to update but always updating is an issue
    inoDisplay.printAmount();
    //inoDisplay.drainBat();
  }
}
