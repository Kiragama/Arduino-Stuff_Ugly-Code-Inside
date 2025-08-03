#include "BLEDevice.h"  //ArduinoBLE version 1.3.1 or below Required
#include <string.h>

#include "InterruptsFile.h"

//_______________________________________________________________________________Interrupt

//_______________________________________________________________________________Interrupt end

// The remote service we wish to connect to. Both should be on the client and server. THIS IS THE CLIENT
static BLEUUID serviceUUID("6bc38e09-5e61-4838-8d1b-f3e093a2c5e6");
// The characteristic of the remote service we are interested in.
static BLEUUID charUUID("b3286334-8358-4b98-9111-2b3cef9758c9");

static BLERemoteCharacteristic* pRemoteCharacteristic;
static BLEAdvertisedDevice* myDevice;
volatile static boolean doConnect = true;  //force compiler to check the value
volatile static boolean connected = false;
static boolean doScan = true;

class MyClientCallback : public BLEClientCallbacks {  //Triggers corresponding function when connection or disconnection
  void onConnect(BLEClient* pclient) {
    attachI();
  }
  void onDisconnect(BLEClient* pclient) {
    connected = false;
    Serial.println("onDisconnect");
    delay(SERIALDELAY);
  }
};

static void notifyCallback(
  BLERemoteCharacteristic* pBLERemoteCharacteristic,
  uint8_t* pData,
  size_t length,
  bool isNotify) {
}

//Scan for BLE servers and find the first one that advertises the service we are looking for.
class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  //Called for each advertising BLE server.
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    if (advertisedDevice.haveName()) {
      Serial.print("BLE Advertised Device found: ");
      Serial.println(advertisedDevice.toString());
      String deviceName = advertisedDevice.getName();  //uses device name over UUID. Saves time as does not need to check Service and then check the characteristics.
      //Serial.print("found: ");
      //Serial.println(deviceName);
      //delay(SERIALDELAY);

      if (deviceName == displayName) {
        Serial.println("name match");
        //delay(SERIALDELAY);
        BLEDevice::getScan()->stop();
        myDevice = new BLEAdvertisedDevice(advertisedDevice);  //set global ptr value to device name
        doConnect = true;
        doScan = true;
        notFound = false;
      } else {
        //Serial.println("device not found");
        notFound = true;
      }
    }

  }  // onResult
};   // MyAdvertisedDeviceCallbacks