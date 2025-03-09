#include <driver/rtc_io.h>
#include "BLEDevice.h" //ArduinoBLE version 1.3.1 or below Required
#include <driver/gpio.h>
#include <string.h>
#include <Arduino.h>   //for multicore
#include "esp_wifi.h"  //to disable wifi

#include "BLEClasses.h"
#include "SleepStuff.h"
#include "disableWifi.h"

TaskHandle_t TaskHandle;
//_______________________________________________________________________________Interrupt
int32_t scancount = 0;

void IRAM_ATTR isr() {                //esp32 specific funtion for interrupts. arduino has set pins while esp can do any. Cannot be any output during interrupt and this includes Serial output
  if (millis() > (last_time + 150))  //4.24 wait | EXAMPLE: 1 millisecond between shots. At 350 FPS, the bb will travel 106mm in a millisecond. (350/1000 (1000  nds in a second) = 0.35 feet. 0.35 is 106mm)
  {
    count++;  //count increments so if multiple shots trigger before upload, it is still counted
    last_time = millis();
  }
}  // NO SERIAL!!!!!!!!!!!!
//_______________________________________________________________________________Interrupt end
//_______________________________________________________________________________Start of BLE5



bool connectToServer() {
  Serial.println("Forming a connection");
  //Serial.println(myDevice->getAddress().toString().c_str());
  BLEClient* pClient = BLEDevice::createClient();
  Serial.println(" - Created client");
  pClient->setClientCallbacks(new MyClientCallback());
  // Connect to the remove BLE Server.
  pClient->connect(myDevice);  // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
  Serial.println(" - Connected to server");
  pClient->setMTU(517);  //set client to request maximum MTU from server (default is 23 otherwise)
  // Obtain a reference to the service we are after in the remote BLE server.
  BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
  if (pRemoteService == nullptr) {
    Serial.print("Failed to find our service UUID: ");
    Serial.println(serviceUUID.toString().c_str());
    pClient->disconnect();
    return false;
  }
  Serial.println(" - Found our service");

  // Obtain a reference to the characteristic in the service of the remote BLE server.
  pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
  if (pRemoteCharacteristic == nullptr) {
    //Serial.print("Failed to find our characteristic UUID: ");
    //Serial.println(charUUID.toString().c_str());
    pClient->disconnect();
    return false;
  }
  Serial.println(" - Found our characteristic");

  if (pRemoteCharacteristic->canNotify())
    pRemoteCharacteristic->registerForNotify(notifyCallback);

  connected = true;
  return true;
}


void doConnectPass(void* pvParameters) {  //*pvParameters is a pointer from the involved class. Passed Parameters to task
  //Serial.println("Task running on core: " + String(xPortGetCoreID()));  //debug
  if (connectToServer()) {
    Serial.println("We are now connected to the BLE Server.");
  } else {
    Serial.println("Failed to connect to the server.");
  }
  vTaskDelete(NULL);  //delete the task to cleanup. Passing NULL deletes itself
}

void setup() {
  Serial.begin(115200);
  delay(1000);  //allow serial to establish.  Debug only
  //disable_wifi();
  pinMode(PIN, INPUT);
  //gpio_deep_sleep_hold_en();                                 //hold the pinmode during sleep
  attachInterrupt(digitalPinToInterrupt(PIN), isr, RISING);  //normal operation interrupt
  last_time = millis();                                      //Initial setup of the time

  //_______________________________________________________________________________START OF BLE5
  //Serial.println("Starting Arduino BLE Client application...");
  BLEDevice::init("Shot tracker reader");  //device name (char limit)
  // Retrieve a Scanner and set the callback we want to use to be informed when we
  // have detected a new device.  Specify that we want active scanning and start the
  // scan to run for 5 seconds.
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5, false);
  //_______________________________________________________________________________END OF BLE5
}

void loop() {
  bool sleepQ = sleepLight();
  //_______________________________________________________________________________ connect to server
  // If the flag "doConnect" is true then we have scanned for and found the desired BLE Server with which we wish to connect.  Now we connect to it.  Once we are connected we set the connected flag to be true.
  //Create Task handle for assigning core 0 a task
  if (doConnect == true) {
    doConnect = false;        //sets do connect to false until the doscan operates
    xTaskCreatePinnedToCore(  //setup/Call task
      doConnectPass,          // Task function
      "ConnectToServer",      // Task name
      10000,                  // Stack size
      NULL,                   // Parameters
      tskIDLE_PRIORITY,       // Priority
      &TaskHandle,            // Task handle
      0                       // Core ID (0 or 1)
    );
  }
  //_______________________________________________________________________________ End of connect to server
  //_______________________________________________________________________________ Do while connected
  if (connected) {  //!sleepQ is a catch to shot bug with first connection not transmitting.
    String value = String(count);
    if (count) {                                                         //not 0 and to not send needless data. Only resets count if transmitted.
      pRemoteCharacteristic->writeValue(value.c_str(), value.length());  //Read by the server
      Serial.println(count);
      count = 0;
      value = "";
    }
  } else if (doScan) {
    if (scancount >= 5) {
      scancount = 0;  //catch so if no server, it goes to sleep after 5 attempts.
      sleepLight();
    } else {
      doConnect = true;
      scancount++;
      BLEDevice::getScan()->start(1);  //default 0 and should be seconds to run for
    }
  }

  //_______________________________________________________________________________ End of Do while connected
  Serial.println("end of loop");
  delay(500);  //transmit every x seconds
}