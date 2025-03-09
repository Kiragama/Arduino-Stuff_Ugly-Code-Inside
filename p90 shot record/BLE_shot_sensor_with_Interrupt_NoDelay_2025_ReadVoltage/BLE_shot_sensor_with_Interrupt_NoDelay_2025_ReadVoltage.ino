#include <driver/rtc_io.h>
#include "BLEDevice.h"
#include <driver/gpio.h>
#include <string.h>
#include "esp_wifi.h"

// The remote service we wish to connect to. Both should be on the client and server. THIS IS THE CLIENT
static BLEUUID serviceUUID("6bc38e09-5e61-4838-8d1b-f3e093a2c5e6");
// The characteristic of the remote service we are interested in.
static BLEUUID charUUID("b3286334-8358-4b98-9111-2b3cef9758c9");

static boolean doConnect = false;
static boolean connected = false;
static boolean doScan = true;
static BLERemoteCharacteristic* pRemoteCharacteristic;
static BLEAdvertisedDevice* myDevice;


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


//_______________________________________________________________________________Interrupt
static byte pin = 0;          //GPIO 0
RTC_DATA_ATTR int count = 0;  //binds it to RTC so does not clear in sleep
unsigned long last_time;

void IRAM_ATTR isr() {  //esp32 specific funtion for interrupts. arduino has set pins while esp can do any. Cannot be any output during interrupt and this includes Serial output
  if (millis() > (last_time + 0.224)) {
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
  Serial.begin(9600);
  pinMode(pin, INPUT_PULLDOWN);
  //delay(200);
  //gpio_light_sleep_hold_en();  //hold the pinmode during sleep
  /*esp_log_level_set("*", ESP_LOG_ERROR);  // set all components to ERROR level. Speeds up wake
  esp_deep_sleep_disable_rom_logging();   // suppress boot messages. Speeds up wake
*/

  //esp_sleep_enable_ext0_wakeup(GPIO_NUM_1, ESP_GPIO_WAKEUP_GPIO_HIGH);  //I am unsure but the pin 0 is GPIO 0 but GPIO 1 triggers. Interrupt
  attachInterrupt(digitalPinToInterrupt(pin), isr, RISING);  //normal operation interrupt

  last_time = millis();  //Initial setup of the time
  //_______________________________________________________________________________End of Sleep
  //_______________________________________________________________________________interrupt for testing reliable reconnects

  disable_wifi();

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

void SleepF() {
  if (millis() > (last_time + 20000))  //if last time, in seconds, is greater then 1+ minutes then sleep. Aka, if idle for 5 minutes then sleep 300000
  {

    //put wifi/bluetooth poweroff here
    esp_bt_controller_disable();

    detachInterrupt(digitalPinToInterrupt(pin));           //keeping an interrupt during sleep causes issues when waking.
    gpio_wakeup_enable(GPIO_NUM_0, GPIO_INTR_HIGH_LEVEL);  //enable this pin to wake


    delay(200);
    esp_sleep_enable_gpio_wakeup();
    Serial.println("wakeup set");
    connected = false;  //sets connected to false as sleep does this and does not update the variable otherwise. Wakups would then think connected and reset the count.
    delay(200);

    //sleep until shot fired
    Serial.println("Sleep");
    esp_light_sleep_start();  //nothing happens till woken and then it continues past this point
    Serial.println("wakeup");
    delay(200);
    isr();  //record woken value and then attach interrupt
    attachInterrupt(digitalPinToInterrupt(pin), isr, RISING);
    last_time = millis();  //sets last time to ensure that it gets set. I know it is set during isr
  }
}
void loop() {
  //SleepF();
  //_______________________________________________________________________________ End of Interrupt and sleep timer
  //_______________________________________________________________________________ connect to server
  // If the flag "doConnect" is true then we have scanned for and found the desired
  // BLE Server with which we wish to connect.  Now we connect to it.  Once we are
  // connected we set the connected flag to be true.
  if (doConnect == true) {
    if (connectToServer()) {
      Serial.println("We are now connected to the BLE Server.");
    } else {
      Serial.println("Failed to connect to the server.");
    }
    doConnect = false;
  }
  //_______________________________________________________________________________ End of connect to server

  // If we are connected to a peer BLE Server, update the characteristic each time we are reached
  //_______________________________________________________________________________ Do while connected
  if (connected) {
    String value = "";
    if (count != 1) {
      value = String(count - 1);  //convers and reduces to fix doouble bug - https://circuits4you.com/2018/03/09/how-to-convert-int-to-string-on-arduino/
    } else {
      value = String(count);  //convers and reduces to fix doouble bug - https://circuits4you.com/2018/03/09/how-to-convert-int-to-string-on-arduino/
    }
    // Set the characteristic's value to be the array of bytes that is actually a string.
    if (count) {                                                         //not 0 and to not send needless data. Only resets count if transmitted.
      pRemoteCharacteristic->writeValue(value.c_str(), value.length());  //Read by the server
      Serial.println(count);
      count = 0;  //reset count after upload
      value = "";
    }
    /*float fval = (((analogRead(0) / 4095.0) * 3.3)); 
    if (fval > 1.6) {
      Serial.println(fval);
    }
*/
  } else if (doScan) {
    BLEDevice::getScan()->start(0);
  }
  //_______________________________________________________________________________ End of Do while connected

  //Serial.println("end of loop");
  delay(1000);  //transmit every x seconds
}
