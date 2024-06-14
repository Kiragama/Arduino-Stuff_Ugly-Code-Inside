/*
    Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleServer.cpp
    Ported to Arduino ESP32 by Evandro Copercini
    updates by chegewara
*/

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <Arduino.h>

//Start of BLE5
// See the following for generating UUIDs:
// https://www.uuidgenerator.net/
#define SERVICE_UUID "6bc38e09-5e61-4838-8d1b-f3e093a2c5e6"  //server advertises and as long as client has the same UUID and Characteristic UUID. Plenty of security holes here
#define CHARACTERISTIC_UUID "b3286334-8358-4b98-9111-2b3cef9758c9"
BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
BLEServerCallbacks* pServerCallbacks = NULL;
bool deviceConnected = false;
int count;
//interrupt for testing reliable reconnects
//hw_timer_t* My_timer = NULL;

//--Classes
class MyCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* pCharacteristic) {
    std::string value = pCharacteristic->getValue();  //Allways gets value as string so better sending as one aswell


    if (value.length() > 0) {
      char tmp[value.length()];
      Serial.print("New value: ");
      for (int i = 0; i < value.length(); i++) {
        tmp[i] = value[i];
      }

      count += atol(tmp);
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

//interrupt for testing reliable reconnects
void IRAM_ATTR onTimer() {
  ESP.restart();
}
//End of BLE5

void setup() {
  //interrupt for testing reliable reconnects
  //My_timer = timerBegin(0, 80, true);
  //timerAttachInterrupt(My_timer, &onTimer, true);
  //timerAlarmWrite(My_timer, 150000000, true);  //every 150 seconds (in microseconds) the device will restart
  //timerAlarmEnable(My_timer);


  Serial.begin(115200);
  //START OF BLE5
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
  //END OF BLE5
}

void loop() {
}