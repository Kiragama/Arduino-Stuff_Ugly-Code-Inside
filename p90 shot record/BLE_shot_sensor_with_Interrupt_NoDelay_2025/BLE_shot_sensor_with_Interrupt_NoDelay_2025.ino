#include <driver/rtc_io.h>
#include "BLEDevice.h" //ArduinoBLE version 1.3.1 or below Required
#include <driver/gpio.h>
#include <string.h>
#include <Arduino.h>   //for multicore
#include "esp_wifi.h"  //to disable wifi

// The remote service we wish to connect to. Both should be on the client and server. THIS IS THE CLIENT
static BLEUUID serviceUUID("6bc38e09-5e61-4838-8d1b-f3e093a2c5e6");
// The characteristic of the remote service we are interested in.
static BLEUUID charUUID("b3286334-8358-4b98-9111-2b3cef9758c9");

static BLERemoteCharacteristic* pRemoteCharacteristic;
static BLEAdvertisedDevice* myDevice;


static boolean doConnect = true;
static boolean connected = false;
static boolean doScan = true;

TaskHandle_t TaskHandle;
//_______________________________________________________________________________Interrupt
#define PIN 18                              //mosi
RTC_DATA_ATTR volatile uint32_t count = 0;  //volatile stops the compiler from assuming the value and actually check. Required if interrupts used
#define uS_TO_S_FACTOR 1000000ULL           //Conversion factor for microseconds seconds to seconds. Used by Sleep timer
#define mS_TO_S_FACTOR 1000ULL
unsigned volatile long last_time;
int32_t scancount = 0;

void disable_wifi() {               //CHATGPT CREATED
  esp_err_t err = esp_wifi_stop();  // Stop Wi-Fi
  if (err == ESP_OK) {
    printf("Wi-Fi disabled successfully\n");
  } else {
    printf("Failed to disable Wi-Fi\n");
  }

  err = esp_wifi_deinit();  // Deinitialize Wi-Fi
  if (err == ESP_OK) {
    printf("Wi-Fi deinitialized successfully\n");
  } else {
    printf("Failed to deinitialize Wi-Fi\n");
  }
}

void IRAM_ATTR isr() {                //esp32 specific funtion for interrupts. arduino has set pins while esp can do any. Cannot be any output during interrupt and this includes Serial output
  if (millis() > (last_time + 150))  //4.24 wait | EXAMPLE: 1 millisecond between shots. At 350 FPS, the bb will travel 106mm in a millisecond. (350/1000 (1000  nds in a second) = 0.35 feet. 0.35 is 106mm)
  {
    count++;  //count increments so if multiple shots trigger before upload, it is still counted
    last_time = millis();
  }
}  // NO SERIAL!!!!!!!!!!!!
//_______________________________________________________________________________Interrupt end
//_______________________________________________________________________________Start of BLE5
static void notifyCallback(
  BLERemoteCharacteristic* pBLERemoteCharacteristic,
  uint8_t* pData,
  size_t length,
  bool isNotify) {
}

class MyClientCallback : public BLEClientCallbacks {  //Triggers corresponding function when connection or disconnection
  void onConnect(BLEClient* pclient) {
  }
  void onDisconnect(BLEClient* pclient) {
    connected = false;
    Serial.println("onDisconnect");
  }
};

//Scan for BLE servers and find the first one that advertises the service we are looking for.
class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  //Called for each advertising BLE server.
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    //Serial.print("BLE Advertised Device found: ");
    //Serial.println(advertisedDevice.toString().c_str());

    // We have found a device, let us now see if it contains the service we are looking for.
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID)) {

      BLEDevice::getScan()->stop();
      myDevice = new BLEAdvertisedDevice(advertisedDevice);
      doConnect = true;
      doScan = true;

    }  // Found our server
  }    // onResult
};     // MyAdvertisedDeviceCallbacks

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


//_______________________________________________________________________________End of BLE5
//_______________________________________________________________________________Sleep
void setSleepwakeup(int sleepTime) {
  gpio_wakeup_enable(GPIO_NUM_35, GPIO_INTR_HIGH_LEVEL);  //enable this pin to wake
  esp_sleep_enable_gpio_wakeup();

  //esp_sleep_enable_timer_wakeup(sleepTime * uS_TO_S_FACTOR);  //sleep time * conversion factor
}
void sleepDeep() {
  Serial.println("DeepSleep");
  esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_TIMER);
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_35, HIGH);
  esp_deep_sleep_start();  //Nothing will happen after this. Will reset on wake
}

bool sleepLight() {
  if (millis() > (last_time + (20 * mS_TO_S_FACTOR)))  //if last time, in seconds, is greater then 1+ minutes then sleep. Aka, if idle for 5 minutes then sleep 300000
  {
    setSleepwakeup(10);  //calls function to set wakeup options. Will sleep until woken by shot or timer. If timer wake then it will go in a deep sleep till woken by shot. Battery saver
    Serial.println("LightSleep");
    esp_light_sleep_start();
    //delay(2000);  //Delay to help Serial. Only for debug but keep commented.
    if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_TIMER) {
      //sleepDeep();  //ESP32 resets code on deep sleep wake
    }
    Serial.println("wakeup");
    count++;  //counts but does not record it
    Serial.println(count);
    connected = false;  //sets connected to false as sleep does this and does not update the variable otherwise. Wakups would then think connected and reset the count.
    doConnect = true;
    last_time = millis();  //sets last time to ensure that it gets set. I know it is set during isr
    return true;           //A check to ensure the connected statement skips the first
  } else {
    return false;  //will be called on the next itteration of loop
  }
}
//_______________________________________________________________________________End of Sleep

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
  //bool sleepQ = sleepLight();
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



/*if (connected && !sleepQ) {  //!sleepQ is a catch to shot bug with first connection not transmitting.
    String value = String(count);
    if (count) {                                                         //not 0 and to not send needless data. Only resets count if transmitted.
      pRemoteCharacteristic->writeValue(value.c_str(), value.length());  //Read by the server
      Serial.println(count);
      count = 0;
      value = "";
    }
  } else if (sleepQ) {
    sleepQ = false;  //catched the statement and sets sleepQ to false. ensures doscan isn't called in the context
  } else if (!sleepQ) {
    //do nothing. If connected false and sleep false then do scan would run
  }
  else if (doScan) {
    if (scancount >= 5) {
      scancount = 0;  //catch so if no server, it goes to sleep after 5 attempts.
      sleepLight();
    } else {
      doConnect = true;
      scancount++;
      BLEDevice::getScan()->start(1);  //default 0 and should be seconds to run for
    }
  }*/