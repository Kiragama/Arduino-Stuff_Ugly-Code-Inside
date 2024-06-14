/*
    Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleServer.cpp
    Ported to Arduino ESP32 by Evandro Copercini
    updates by chegewara
*/

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID "6bc38e09-5e61-4838-8d1b-f3e093a2c5e6"
#define CHARACTERISTIC_UUID "b3286334-8358-4b98-9111-2b3cef9758c9"

BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
BLEServerCallbacks* pServerCallbacks = NULL;
bool deviceConnected = false;

class MyCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* pCharacteristic) {
    std::string value = pCharacteristic->getValue();

    if (value.length() > 0) {
      Serial.println("*********");
      Serial.print("New value: ");
      for (int i = 0; i < value.length(); i++)
        Serial.print(value[i]);

      Serial.println();
      Serial.println("*********");
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



void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE work!");

  BLEDevice::init("Long name works now");
  pServer = BLEDevice::createServer();
  BLEService* pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);

  pCharacteristic->setValue("Hello World says Neil");
  pService->start();
  // BLEAdvertising *pAdvertising = pServer->getAdvertising();  // this still is working for backward compatibility
  BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("Characteristic defined! Now you can read it in your phone!");
  pServer->setCallbacks(new MyServerCallback());
  pCharacteristic->setCallbacks(new MyCallbacks());
}

void loop() {
  /*
  if (deviceConnected) {
    //do stuff when connected like sending data
    delay(10);  // bluetooth stack will go into congestion, if too many packets are sent, in 6 hours test i was able to go as low as 3ms
  }

  // disconnecting
  if (!deviceConnected) {
    delay(500);                   // give the bluetooth stack the chance to get things ready
    pServer->startAdvertising();  // restart advertising
    Serial.println("start advertising");
  }
  // connecting
  if (deviceConnected) {
    // do stuff here on connecting
    delay(2000);
  }*/
}