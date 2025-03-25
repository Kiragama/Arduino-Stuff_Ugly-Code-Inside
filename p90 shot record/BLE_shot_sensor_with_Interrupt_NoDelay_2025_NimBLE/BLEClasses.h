#include "NimBLEDevice.h"  //ArduinoBLE version 1.3.1 or below Required
#include <string.h>

#define PIN 35
#define TRACKERNAME "Shot tracker Display"
unsigned volatile long last_time;
RTC_DATA_ATTR volatile uint32_t count = 0;  //volatile stops the compiler from assuming the value and actually check. Required if interrupts used

//_______________________________________________________________________________Interrupt
void IRAM_ATTR isr() {                //esp32 specific funtion for interrupts. arduino has set pins while esp can do any. Cannot be any output during interrupt and this includes Serial output
  if (millis() > (last_time + 150))  //4.24 wait | EXAMPLE: 1 millisecond between shots. At 350 FPS, the bb will travel 106mm in a millisecond. (350/1000 (1000  nds in a second) = 0.35 feet. 0.35 is 106mm)
  {
    count++;  //count increments so if multiple shots trigger before upload, it is still counted
    last_time = millis();
  }
}  // NO SERIAL!!!!!!!!!!!!
//_______________________________________________________________________________Interrupt end

// The remote service we wish to connect to. Both should be on the client and server. THIS IS THE CLIENT
static BLEUUID serviceUUID("6bc38e09-5e61-4838-8d1b-f3e093a2c5e6");
// The characteristic of the remote service we are interested in.
static BLEUUID charUUID("b3286334-8358-4b98-9111-2b3cef9758c9");

static BLERemoteCharacteristic* pRemoteCharacteristic;
static BLEAdvertisedDevice* myDevice;
volatile static boolean doConnect = true; //force compiler to check the value
volatile static boolean connected = false;
static boolean doScan = true; 

class MyClientCallback : public BLEClientCallbacks {  //Triggers corresponding function when connection or disconnection
  void onConnect(BLEClient* pclient) {
    attachInterrupt(digitalPinToInterrupt(PIN), isr, RISING); //enable input
  }
  void onDisconnect(BLEClient* pclient) {
    connected = false;
    Serial.println("onDisconnect");
  }
};

void notifyCB(NimBLERemoteCharacteristic* pRemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify) {
    std::string str  = (isNotify == true) ? "Notification" : "Indication";
    str             += " from ";
    str             += pRemoteCharacteristic->getClient()->getPeerAddress().toString();
    str             += ": Service = " + pRemoteCharacteristic->getRemoteService()->getUUID().toString();
    str             += ", Characteristic = " + pRemoteCharacteristic->getUUID().toString();
    str             += ", Value = " + std::string((char*)pData, length);
    Serial.printf("%s\n", str.c_str());
}


//Scan for BLE servers and find the first one that advertises the service we are looking for.
class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  //Called for each advertising BLE server.
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    //Serial.print("BLE Advertised Device found: ");
    //Serial.println(advertisedDevice.toString().c_str());

    // We have found a device, let us now see if it contains the service we are looking for. Ideal if device name varies
    /*if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID)) {

      BLEDevice::getScan()->stop();
      myDevice = new BLEAdvertisedDevice(advertisedDevice);
      doConnect = true;
      doScan = true;

    }  // Found our server*/
    std::string deviceName = advertisedDevice.getName(); //uses device name over UUID. Saves time as does not need to check Service and then check the characteristics.
    if (deviceName == TRACKERNAME) {
      Serial.println("name match");
      BLEDevice::getScan()->stop();
      myDevice = new BLEAdvertisedDevice(advertisedDevice); //set global ptr value to device name
      doConnect = true;
      doScan = true;
    }
  }  // onResult
};   // MyAdvertisedDeviceCallbacks