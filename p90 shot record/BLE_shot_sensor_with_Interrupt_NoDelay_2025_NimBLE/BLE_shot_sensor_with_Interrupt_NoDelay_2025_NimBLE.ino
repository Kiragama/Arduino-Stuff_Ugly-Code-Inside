#include <driver/rtc_io.h>
#include "NimBLEDevice.h"  //ArduinoBLE version 1.3.1 or below Required
#include <driver/gpio.h>
#include <string.h>
#include <Arduino.h>        //for multicore
#include "esp_wifi.h"       //to disable wifi
static BLEClient* pClient;  //initialise global before Sleepstuff included allows for that file to access it.

#include "BLEClasses.h"
#include "SleepStuff.h"
#include "disableWifi.h"

TaskHandle_t TaskHandle; //DOES NOT WORK. JUST LOAD THE NIMBLE CLIENT EXAMPLE AND COPYPASTE

int32_t scancount = 0;
bool serverconnecting = false;

#define EN A2
//_______________________________________________________________________________Start of BLE5

bool connectToServer() {

  int delayVal = 100;  //having a delay to this allows for everything to load. This matters as an interrupt will halt the process and break it.
  Serial.println("Forming a connection");
  //Serial.println(myDevice->getAddress().toString().c_str());
  static BLEClient* pClient = BLEDevice::createClient();
  Serial.println(" - Created client");
  delay(delayVal);
  pClient->setClientCallbacks(new MyClientCallback());
  // Connect to the remove BLE Server.
  pClient->connect(myDevice);  // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
  Serial.println(" - Connected to server");
  delay(delayVal);
  //pClient->setMTU(517);  //set client to request maximum MTU from server (default is 23 otherwise)
  // Obtain a reference to the service we are after in the remote BLE server.
  BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
  if (pRemoteService == nullptr) {
    Serial.print("Failed to find our service UUID: ");
    Serial.println(serviceUUID.toString().c_str());
    pClient->disconnect();
    return false;
  }
  Serial.println(" - Found our service");
  delay(delayVal);
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
    if (!pChr->subscribe(true, notifyCB)) {
                    pClient->disconnect();
                    return false;

  delay(delayVal);
  connected = true;
  doScan = true;  //No longer need to worry about scans happening during connection
  return true;
}

void doConnectPass(void* pvParameters) {  //*pvParameters is a pointer from the involved class. Passed Parameters to task
  //Serial.println("Task running on core: " + String(xPortGetCoreID()));  //debug
  if (connectToServer()) {
    Serial.println("We are now connected to the BLE Server.");
  } else {
    Serial.println("Failed to connect to the server.");
    doScan = true;  //allows scans again
  }
  vTaskDelete(NULL);  //delete the task to cleanup. Passing NULL deletes itself
}

void setup() {
  Serial.begin(115200);
  delay(1000);  //allow serial to establish.  Debug only
  //disable_wifi();
  pinMode(PIN, INPUT);
  pinMode(EN, INPUT_PULLDOWN); //pulldown to reduce noise and accidental reads
  //gpio_deep_sleep_hold_en();                                 //hold the pinmode during sleep
  attachInterrupt(digitalPinToInterrupt(PIN), isr, RISING);  //normal operation interrupt
  last_time = millis();                                      //Initial setup of the time
  //_______________________________________________________________________________START OF BLE5
  //Serial.println("Starting Arduino BLE Client application...");
  BLEDevice::init("Shot tracker client");  //device name (char limit)
  // Retrieve a Scanner and set the callback we want to use to be informed when we
  // have detected a new device.  Specify that we want active scanning and start the
  // scan to run for 5 seconds.
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  BLEDevice::setPower(ESP_PWR_LVL_P9);  //9db power. Default is 3
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5, false);
  //_______________________________________________________________________________END OF BLE5
}

void loop() {
  while (!digitalRead(EN)) { //the pin reads 0. If EN not used then no change. Used for checking if tracer awake or not
    sleepLight(last_time, count);
    static int loopcount = 0;
    //_______________________________________________________________________________ connect to server
    // If the flag "doConnect" is true then we have scanned for and found the desired BLE Server with which we wish to connect.  Now we connect to it.  Once we are connected we set the connected flag to be true.
    //Create Task handle for assigning core 0 a task
    if (doConnect == true) {
      detachInterrupt(digitalPinToInterrupt(PIN));  //interrupts during connection causes it to reset
      doConnect = false;                            //sets do connect to false until the doscan operates
      //Serial.println("doconnect false");
      doScan = false;           //stops scans while connecting.
      xTaskCreatePinnedToCore(  //setup/Call task
        doConnectPass,          // Task function
        "ConnectToServer",      // Task name
        10000,                  // Stack size
        NULL,                   // Parameters
        tskIDLE_PRIORITY,       // Priority
        &TaskHandle,            // Task handle
        0                       // Core ID (0 or 1)
      );
      //attaching interrupt here does nothing. Must attach in function
    }
    //_______________________________________________________________________________ End of connect to server
    //_______________________________________________________________________________ Do while connected
    if (connected) {
      String value = String(count);
      if (count) {                                                         //not 0 and to not send needless data. Only resets count if transmitted.
        pRemoteCharacteristic->writeValue(value.c_str(), value.length());  //Read by the server
        Serial.println(count);
        count = 0;
        value = "";
        loopcount = 0;
      }
      //int rssi = pClient->getRssi();
      //Serial.println(String(rssi));
    } else if (doScan) {  //if other core is not activily connecting and scanning enabled
      if (scancount >= 5) {
        scancount = 0;  //catch so if no server, it goes to sleep after 5 attempts.
        loopcount = 0;
        sleepLight(last_time, count);
      } else {
        doConnect = true;
        loopcount = 0;
        Serial.println("doconnect true");
        scancount++;
        BLEDevice::getScan()->start(1);  //default 0 and seconds to run for
      }
    } else if (loopcount >= 2 && !doScan && !connected) {
      doScan = true;
      loopcount = 0;
    }


    //_______________________________________________________________________________ End of Do while connected

    Serial.println("end of loop");
    delay(500);  //transmit every x seconds
  }
  //loop broken as En is not 0
  static bool tracerSleep = true; //a flag to record if tracer slept. Useful to know and stop false triggers. When the tracer wakes, it triggers int.
  sleepLight(last_time, count); //go to sleep with tracer
  
}