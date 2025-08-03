#include <Arduino.h>  //for multicore
#include <driver/rtc_io.h>
#include "BLEDevice.h"  //ArduinoBLE version 1.3.1 or below Required
#include <driver/gpio.h>
#include <string.h>
#include "Defines.h"  //must be first to be included

static BLEClient* pClient;  //initialise global before Sleepstuff included allows for that file to access it.

#include "BLEClasses.h"
#include "SleepStuff.h"

#if USING_OMGS3
#include "OMGbatteryCheck.h"
#endif

#include "disableWifi.h"


TaskHandle_t TaskHandle;

#include "Fucntions_Classes.h"


void setup() {
  Serial.begin(9600);  //comment this on final
  delay(1000);         //allow serial to establish.  Debug only
  disable_wifi();
  pinMode(PIN, INPUT_PULLDOWN);
  pinMode(WAKEPIN, INPUT_PULLDOWN);
  pinMode(PAIRBTN, INPUT_PULLUP);
  attachI();
  last_time = millis();  //Initial setup of the time
  last_transmit = millis();

//<OMGSETUP>
#if USING_OMGS3
  ums3.begin();
  // Brightness is 0-255. We set it to 1/3 brightness here
  ums3.setPixelBrightness(255 / 3);
  // We initialise the I2C peripheral outside of the helper library and pass the reference in
  // In case you want to use the BUS for other I2C peripherals as well.
  Wire.begin();
  ums3.FG_setup(Wire);
#endif
  //</OMGSETUP>

  //<BLE5>
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
  //</BLE5>
  delay(1000);
}


void loop() {

  connectLoop();
#if USING_OMGS3  //Done this was as to stop the compiler from looking at contents when OMG disabled
  if ((lastBatteryCheck == 0 || millis() - lastBatteryCheck > BATTERY_CHECK_INTERVAL)) {
    checkBattery();
    lastBatteryCheck = millis();
  }
#endif
  static int loopcount = 0;
  //_______________________________________________________________________________ connect to server
  // If the flag "doConnect" is true then we have scanned for and found the desired BLE Server with which we wish to connect.  Now we connect to it.  Once we are connected we set the connected flag to be true.
  //Create Task handle for assigning core 0 a task
  if (doConnect == true) {
    doConnect = false;  //sets do connect to false until the doscan operates
    //Serial.println("doconnect false");
    doScan = false;           //stops scans while connecting.
    xTaskCreatePinnedToCore(  //setup/Call task
      doConnectPass,          // Task function
      "ConnectToServer",      // Task name
      10000,                  // Stack size
      NULL,                   // Parameters
      tskIDLE_PRIORITY,       // Priority
      &TaskHandle,            // Task handle
      0                       // Core ID (0 or 1) | Core 0 normally handles internal code, bluetooth, and wifi. Core 1 is normal program. Assigning bluetooth to 0 is fine but running BT on core 1, while using core 0 caused deadlock.
    );
    //attaching interrupt here does nothing. Must attach in function
  }
  //_______________________________________________________________________________ End of connect to server
  //_______________________________________________________________________________ Do while connected
  if (connected && TRANSMIT) {
    String value = String(count);
    //if (count) {
    if (millis() > (last_transmit + 100)) {                              //run normally
      pRemoteCharacteristic->writeValue(value.c_str(), value.length());  //Read by the server
      if (PRINTING) {
        Serial.print(count);
        Serial.println(" Sent");
      }
      count = 0;
      value = "";
      loopcount = 0;
      last_transmit = millis();
    }
    sleepLight(false);
  } else if (doScan && TRANSMIT) {  //if other core is not activily connecting and scanning enabled
    if (scancount >= 3) {
      scancount = 0;  //catch so if no server, it goes to sleep after 5 attempts.
      loopcount = 0;
      //sleepLight(true);  //pass true to overide the delay
      ESP.restart();
    } else {
      doConnect = true;
      loopcount = 0;
      Serial.println("doconnect true");
      scancount++;
      BLEDevice::getScan()->start(1);  //default 0 and seconds to run for
    }
  } else if (loopcount >= 2 && !doScan && !connected && TRANSMIT) {
    doScan = true;  //a way for doscan to reactivate
    loopcount = 0;
  }
  //_______________________________________________________________________________ End of Do while connected


  if (PRINTING) {
    //Serial.println("end of loop");
  }
  delay(DELAYFACTOR);  //transmit every x seconds
}