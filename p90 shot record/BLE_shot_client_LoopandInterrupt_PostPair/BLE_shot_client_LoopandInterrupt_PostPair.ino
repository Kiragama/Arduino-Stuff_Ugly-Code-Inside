/*

Author: Kiragama

Notes:
To pair, Press pair on display, wait 5 seconds, press pair on sensor.

There is no delay or main reason for this other then to make it more reliable. If not paired after 30+ seconds. Press pair on sensor again.
Power cycling will remove pairMode.
###################################################################################
*/
#include <Arduino.h>  //for multicore
#include <driver/rtc_io.h>
#include "BLEDevice.h"  //ArduinoBLE version 1.3.1 or below Required
#include <driver/gpio.h>
#include <string.h>
#include "Defines.h"  //must be first to be included

static BLEClient* pClient;  //initialise global before Sleepstuff included allows for that file to access it.

#include "Use_EEPROM.h"



Use_EEPROM inoee;
#include "BLEClasses.h"
#include "SleepStuff.h"
/*
#if USING_OMGS3
#include "OMGbatteryCheck.h"
#include <Wire.h>
#endif*/

#include "disableWifi.h"


TaskHandle_t TaskHandle;

#include "Fucntions_Classes.h"


void setup() {
  if (PRINTING) {
    Serial.begin(115200);  //comment this on final
  }
  disable_wifi();
  EEPROM.begin(EEPROM_SIZE);
  pinMode(PIN, INPUT);
  pinMode(WAKEPIN, INPUT);
  pinMode(PAIRBTN, INPUT_PULLUP);
  attachI();
  last_time = millis();  //Initial setup of the time
  last_transmit = millis();

  //<OMGSETUP>
  /*#if USING_OMGS3
  ums3.begin();
  // Brightness is 0-255. We set it to 1/3 brightness here
  ums3.setPixelBrightness(255 / 3);
  // We initialise the I2C peripheral outside of the helper library and pass the reference in
  // In case you want to use the BUS for other I2C peripherals as well.
  Wire.begin();
  ums3.FG_setup(Wire);
#endif*/
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
  //</BLE5> - MUST BE DEFINED BEFORE PAIRMODE AND NAME SET

  if (USING_PAIRMODE) {
    float tmpDisplayIn = inoee.loadDis(NAMEADDRESS);

    if (tmpDisplayIn == DEFAULTP) {  //1.23 is used to determin if the Shot tracker Display has been reset to default.

      displayName = TRACKERNAME;   //resets to default so a sensor can connect and send their pair value.
      restartedInPairMode = true;  //as it reboots, this allows it to be known elsewhere
      Serial.println("Rebooted in Pairmode");
      Serial.println("Default Server Name");
      Serial.print("Loading pairValue, ");
      delay(SERIALDELAY);
      pairValue = inoee.loadDis(RANDADDRESS);  //loads pairvalue
      inoee.saveDis(0.00, NAMEADDRESS);        //once happened, reset that address for next reboot (to run normally)

    } else {
      displayName = "Shot_Tracker_Display_Tag-" + String(int(inoee.loadDis(RANDADDRESS)));  //converts loaded value from float to int as it removes the .00, which causes the name to not be detected. Then converted to string for transport
      Serial.print("Loaded Server Name, It is: ");
      Serial.println(displayName);
      delay(SERIALDELAY);
    }
  } else {
    displayName = TRACKERNAME;
    Serial.print("Loaded Default Server Name: ");
    Serial.println(displayName);
    delay(SERIALDELAY);
  }
  delay(1000);
}
//if trouble with connecting on or after pair. Server must be up before


void loop() {

  connectLoop();
#if USING_OMGS3  //Done this was as to stop the compiler from looking at contents when OMG disabled
  if ((lastBatteryCheck == 0 || millis() - lastBatteryCheck > BATTERY_CHECK_INTERVAL)) {
    //checkBattery();
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
    if (millis() > (last_transmit + 100)) {
      if (restartedInPairMode && pairTime <= PAIRTIMER) {  //if not in pairmode, then work normally. Otherwise, send the randomly generated pair value. Stay in pairMode for a set number of sends
        value = String(pairValue);
        pRemoteCharacteristic->writeValue(value.c_str(), value.length());
        pairTime++;
        if (PRINTING) {
          Serial.print("pair value is:");
          Serial.println(value);
          Serial.println("sent");
          Serial.print("pairTime Count is: ");
          Serial.println(pairTime);
          delay(SERIALDELAY);
        }
        inoee.reboot();
      } else if (pairTime >= PAIRTIMER) {
        //inoee.reboot(); //when timer runs out, reboot to allow the name change of this device to match the display.

      } else {                                                             //run normally
        pRemoteCharacteristic->writeValue(value.c_str(), value.length());  //Read by the server
        if (PRINTING) {
          Serial.print(count);
          Serial.println(" Sent");
          delay(SERIALDELAY);
        }

        count = 0;
        value = "";
        loopcount = 0;
        last_transmit = millis();
      }
    }
    sleepLight(false);
  } else if (doScan && TRANSMIT) {  //if other core is not activily connecting and scanning enabled
    if (scancount >= 10) {
      scancount = 0;  //catch so if no server, it goes to sleep after 5 attempts.
      loopcount = 0;
      //sleepLight(true);  //pass true to overide the delay
      inoee.reboot();
    } else {
      doConnect = true;
      loopcount = 0;
      Serial.println("doconnect true");
      delay(SERIALDELAY);
      scancount++;
      BLEDevice::getScan()->start(1);  //default 0 and seconds to run for
    }
  } else if (loopcount >= 2 && !doScan && !connected && TRANSMIT) {
    doScan = true;  //a way for doscan to reactivate
    loopcount = 0;
  }
  //_______________________________________________________________________________ End of Do while connected

  if (isInPairMode && USING_PAIRMODE) {     //this is a check for if the interupt has  been triggered. This does not happen in interrupt as writing to EEPROM would crash esp
    inoee.saveDis(DEFAULTP, NAMEADDRESS);   //0.00 used to flag the default
    inoee.saveDis(pairValue, RANDADDRESS);  //saves pair value to be reused on startup
    Serial.println("Pair");
    delay(750);      //for debug and can actually see the print
    inoee.reboot();  //reboots so on setup, the name is default. The sensor will connect and then send the pair value.
  }


  if (PRINTING) {
    //Serial.println("end of loop");
  }
  delay(DELAYFACTOR);  //transmit every x seconds
}