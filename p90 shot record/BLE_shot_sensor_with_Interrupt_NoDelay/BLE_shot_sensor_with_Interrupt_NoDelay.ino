#include <driver/rtc_io.h>
#include "BLEDevice.h"
#include <driver/gpio.h>
#include <string.h>


// The remote service we wish to connect to. Both should be on the client and server. THIS IS THE CLIENT
static BLEUUID serviceUUID("6bc38e09-5e61-4838-8d1b-f3e093a2c5e6");
// The characteristic of the remote service we are interested in.
static BLEUUID charUUID("b3286334-8358-4b98-9111-2b3cef9758c9");

static boolean doConnect = false;
static boolean connected = false;
static boolean doScan = true;
static BLERemoteCharacteristic* pRemoteCharacteristic;
static BLEAdvertisedDevice* myDevice;

//_______________________________________________________________________________Interrupt
static byte pin = 0;  //GPIO 0
RTC_DATA_ATTR int count = 0;
unsigned long last_time;

/*testing shows following on full auto
    9/9
    9/9
    15/15
    This is tested with output directly from the sensor and not over BT.
    */
void IRAM_ATTR isr() {             //esp32 specific funtion for interrupts. arduino has set pins while esp can do any. Cannot be any output during interrupt and this includes Serial output
  if (millis() > (last_time + 1))  //wait 1 millisecond between shots. At 350 FPS, the bb will travel 106mm in a millisecond. (350/1000 (1000 milliseconds in a second) = 0.35 feet. 0.35 is 106mm)
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
    //Serial.println("onDisconnect");
  }
};

bool connectToServer() {

  //Serial.print("Forming a connection to ");
  //Serial.println(myDevice->getAddress().toString().c_str());
  BLEClient* pClient = BLEDevice::createClient();
  //Serial.println(" - Created client");
  pClient->setClientCallbacks(new MyClientCallback());
  // Connect to the remove BLE Server.
  pClient->connect(myDevice);  // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
  //Serial.println(" - Connected to server");
  pClient->setMTU(517);  //set client to request maximum MTU from server (default is 23 otherwise)
  // Obtain a reference to the service we are after in the remote BLE server.
  BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
  if (pRemoteService == nullptr) {
    //Serial.print("Failed to find our service UUID: ");
    //Serial.println(serviceUUID.toString().c_str());
    pClient->disconnect();
    return false;
  }
  //Serial.println(" - Found our service");


  // Obtain a reference to the characteristic in the service of the remote BLE server.
  pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
  if (pRemoteCharacteristic == nullptr) {
    //Serial.print("Failed to find our characteristic UUID: ");
    //Serial.println(charUUID.toString().c_str());
    pClient->disconnect();
    return false;
  }
  //Serial.println(" - Found our characteristic");

  // Read the value of the characteristic.
  if (pRemoteCharacteristic->canRead()) {
    String value = pRemoteCharacteristic->readValue();
    //Serial.print("The characteristic value was: ");
    //Serial.println(value.c_str());
  }

  if (pRemoteCharacteristic->canNotify())
    pRemoteCharacteristic->registerForNotify(notifyCallback);

  connected = true;
  return true;
}
/**
 * Scan for BLE servers and find the first one that advertises the service we are looking for.
 */
class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  /**
   * Called for each advertising BLE server.
   */
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
//_______________________________________________________________________________End of BLE5

void setup() {
  //_______________________________________________________________________________Sleep
  pinMode(pin, INPUT);
  gpio_light_sleep_hold_en();  //hold the pinmode during sleep
  /*esp_log_level_set("*", ESP_LOG_ERROR);  // set all components to ERROR level. Speeds up wake
  esp_deep_sleep_disable_rom_logging();   // suppress boot messages. Speeds up wake
*/

  if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_GPIO) {

    isr();  //if woken from sleep, then call interrupt
  }

  esp_sleep_disable_bt_wakeup();
  esp_sleep_disable_wifi_wakeup();
  esp_sleep_enable_gpio_wakeup();
  
  esp_sleep_enable_ext1_wakeup(GPIO_NUM_1, ESP_GPIO_WAKEUP_GPIO_HIGH);  //I am unsure but the pin 0 is GPIO 0 but GPIO 1 triggers. Interrupt
  attachInterrupt(digitalPinToInterrupt(pin), isr, RISING);                  //normal operation interrupt

  last_time = millis();  //Initial setup of the time
  //_______________________________________________________________________________End of Sleep
  //_______________________________________________________________________________interrupt for testing reliable reconnects


  Serial.begin(9600);
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

  //_______________________________________________________________________________Interrupt and sleep timer
  if (millis() > (last_time + 300000))  //if last time, in seconds, is greater then 1+ minutes then sleep. Aka, if idle for 5 minutes then sleep
  {

    //Serial.println("tRIGGER");
    //put wifi/bluetooth poweroff here
    esp_bt_controller_disable();

    //sleep until shot fired
    esp_light_sleep_start();
  }
  //_______________________________________________________________________________ End of Interrupt and sleep timer
  //_______________________________________________________________________________ connect to server
  // If the flag "doConnect" is true then we have scanned for and found the desired
  // BLE Server with which we wish to connect.  Now we connect to it.  Once we are
  // connected we set the connected flag to be true.
  if (doConnect == true) {
    if (connectToServer()) {
      //Serial.println("We are now connected to the BLE Server.");
    } else {
      //Serial.println("Failed to connect to the server.");
    }
    doConnect = false;
  }
  //_______________________________________________________________________________ End of connect to server

  // If we are connected to a peer BLE Server, update the characteristic each time we are reached
  //_______________________________________________________________________________ Do while connected
  if (connected) {
    //String newValue = "Time since boot: " + String(millis() / 100000);
    String value = String(count);  //convert https://circuits4you.com/2018/03/09/how-to-convert-int-to-string-on-arduino/
    //Serial.println("Setting new characteristic value to \"" + newValue + "\"");  //just sending data
    // Set the characteristic's value to be the array of bytes that is actually a string.
    //pRemoteCharacteristic->writeValue(newValue.c_str(), newValue.length());  //Read by the server
    if (count) {                                                         //not 0 and to not send needless data. Only resets count if transmitted.
      pRemoteCharacteristic->writeValue(value.c_str(), value.length());  //Read by the server
      Serial.println(count);
      //Serial.println(value);
      count = 0;  //reset count after upload
      value = "";
    }
  } else if (doScan) {
    BLEDevice::getScan()->start(0);
  }
  //_______________________________________________________________________________ End of Do while connected

  delay(1000);  //transmit every x seconds
}
