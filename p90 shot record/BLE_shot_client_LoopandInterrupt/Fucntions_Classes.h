bool connectToServer() {  //Full process for connecting to the server. It will detach interrupts at certain points to prevent breakage but mainly creates client and scans for the matching device name.
  int delayVal = 500;     //having a delay to this allows for everything to load.
  Serial.println("Forming a connection");
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
  BLERemoteService* pRemoteService = pClient->getService(serviceUUID);  //checks for UUID but connects to matching server.
  if (pRemoteService == nullptr) {
    Serial.print("Failed to find our service UUID: ");
    Serial.flush();
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
  Serial.flush();

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
  Serial.flush();
  vTaskDelete(NULL);  //delete the task to cleanup. Passing NULL deletes itself
}


void connectLoop() {
  //instead of interrupts, the reading is passed to another core. Should reduce read delays.
  //Reasoning: if a pin remains high (touch does not count) then it will not trigger again until that pin drains. The tracer remains above the trigger threshold if shots are too close to eachother. Because interrupt is triggerred the once, shots are missed.
  //Serial.println("Task running on core: " + String(xPortGetCoreID()));  //debug
  if (!USING_INTERRUPT && millis() >= (last_time + DELAYFACTOR)) {
    count += 1;
    last_time = millis();
    Serial.print(count);
    Serial.print(" is recorded\n");
  }
}