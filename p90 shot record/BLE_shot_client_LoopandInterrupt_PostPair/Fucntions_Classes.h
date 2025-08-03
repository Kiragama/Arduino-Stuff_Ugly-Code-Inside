bool connectToServer() {  //Full process for connecting to the server. It will detach interrupts at certain points to prevent breakage but mainly creates client and scans for the matching device name.
  Serial.println("Forming a connection");
  pClient = BLEDevice::createClient();
  Serial.println(" - Created client");
  //delay(SERIALDELAY);

  if (notFound) {
    attachI();
    return false;
  }
  attachI();
  pClient->setClientCallbacks(new MyClientCallback());
  // Connect to the remove BLE Server.
  pClient->connect(myDevice);
  //delay(1000);
  // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
  Serial.println(" - Connected to server");


  pClient->setMTU(517);  //set client to request maximum MTU from server (default is 23 otherwise)
  // Obtain a reference to the service we are after in the remote BLE server.
  BLERemoteService* pRemoteService = pClient->getService(serviceUUID);  //checks for UUID but connects to matching server.
  if (pRemoteService == nullptr) {
    Serial.print("Failed to find our service UUID: ");
    //Serial.flush();
    Serial.println(serviceUUID.toString().c_str());
    pClient->disconnect();
    //delay(SERIALDELAY);
    return false;
  }
  Serial.println(" - Found our service");
  //delay(SERIALDELAY);
  // Obtain a reference to the characteristic in the service of the remote BLE server.
  pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
  if (pRemoteCharacteristic == nullptr) {
    Serial.print("Failed to find our characteristic UUID: ");
    //Serial.println(charUUID.toString().c_str());
    pClient->disconnect();
    return false;
  }
  Serial.println(" - Found our characteristic");
  //delay(SERIALDELAY);

  if (pRemoteCharacteristic->canNotify())
    pRemoteCharacteristic->registerForNotify(notifyCallback);
  //delay(SERIALDELAY);
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
    //delay(6000);
    doScan = true;  //allows scans again
  }
  //delay(SERIALDELAY);
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
    delay(SERIALDELAY);
  }
}