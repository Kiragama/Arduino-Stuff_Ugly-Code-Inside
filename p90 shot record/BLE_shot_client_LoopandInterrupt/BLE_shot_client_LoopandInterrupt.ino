#include <driver/rtc_io.h>
#include "BLEDevice.h"  //ArduinoBLE version 1.3.1 or below Required
#include <driver/gpio.h>
#include <string.h>
#include <Arduino.h>        //for multicore
#include "esp_wifi.h"       //to disable wifi
static BLEClient* pClient;  //initialise global before Sleepstuff included allows for that file to access it.

#include "BLEClasses.h"
#include "SleepStuff.h"
#include "disableWifi.h"


TaskHandle_t TaskHandle;

//_______________________________________________________________________________Start of BLE5

bool connectToServer() {
  int delayVal = 500;  //having a delay to this allows for everything to load. This matters as an interrupt will halt the process and break it.
  Serial.println("Forming a connection");
  //Serial.println(myDevice->getAddress().toString().c_str());
  pClient = BLEDevice::createClient();
  Serial.println(" - Created client");
  delay(delayVal);
  if (notFound) {
    attachI();
    return false;
  }
  attachI();
  pClient->setClientCallbacks(new MyClientCallback());
  // Connect to the remove BLE Server.
  pClient->connect(myDevice);
  // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
  Serial.println(" - Connected to server");
  delay(delayVal);
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
    pRemoteCharacteristic->registerForNotify(notifyCallback);

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


float calcVolt() {
  //float in = analogRead(PIN);
  float refVolt = 3.3;
  float adc = 4095.0;
  //float samples = 30;
  float sum = 0;

  /*for (int i = 0; i < samples; i++) {
    sum += analogRead(PIN);
    delayMicroseconds(200);
  }*/

  //float avg_adc = sum / samples;
  //float result = (avg_adc / adc) * refVolt;  // Scale to voltage

  int raw_adc = analogRead(PIN);         // Read raw ADC value
  float voltage = (raw_adc / adc) * refVolt;  // Convert to voltage (V)

  return voltage;
}

void connectLoop() {
  //instead of interrupts, the reading is passed to another core. Should reduce read delays.
  //Reasoning: if a pin remains high (touch does not count) then it will not trigger again until that pin drains. The tracer remains above the trigger threshold if shots are too close to eachother. Because interrupt is triggerred the once, shots are missed.
  //Serial.println("Task running on core: " + String(xPortGetCoreID()));  //debug
  /*if (!USING_INTERRUPT && calcVolt() >= 0.5 && millis() >= (last_time + DELAYFACTOR)) {
    count += 1;
    last_time = millis();
    Serial.print(count);
    Serial.print(" is recorded\n");
  }*/
  if (millis() >= (last_time + DELAYFACTOR)) {
    float result = calcVolt();
    if (result >= 0.45) {
      Serial.println(result);
    }
  }
}












void setup() {
  Serial.begin(9600);  //comment this on final
  delay(1000);         //allow serial to establish.  Debug only
  //disable_wifi();
  pinMode(PIN, INPUT_PULLDOWN);
  pinMode(WAKEPIN, INPUT_PULLDOWN);
  //gpio_deep_sleep_hold_en();                                 //hold the pinmode during sleep
  attachI();
  last_time = millis();  //Initial setup of the time
  last_transmit = millis();

  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);


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

  connectLoop();


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
      pRemoteCharacteristic->writeValue(value.c_str(), value.length());  //Read by the server
      Serial.print(count);
      Serial.println(" Sent");
      count = 0;
      value = "";
      loopcount = 0;
      last_transmit = millis();
      //}
    }
    sleepLight(false);
  } else if (doScan && TRANSMIT) {  //if other core is not activily connecting and scanning enabled
    if (scancount >= 5) {
      scancount = 0;  //catch so if no server, it goes to sleep after 5 attempts.
      loopcount = 0;
      sleepLight(true);  //pass true to overide the delay
      //ESP.restart();  //Restart device. Loses the stored value but allow for more reliable reconnect to the Py shot tracker.
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

  //Serial.print(count);
  //Serial.println("end of loop");
  delay(DELAYFACTOR);  //transmit every x seconds
}