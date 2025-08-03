#pragma once

//FUNCTIONS
void tickDown(int count) {
  for (int i = 0; i < count; i++) {
    inoDisplay.setammo(inoDisplay.getammo() - 1);  //reduce ammo by one per count
    inoDisplay.printAmount();
    delay(10);  //so it appears to tick down
  }
}

//CLASSES
class MyCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* pCharacteristic) {
    String value = pCharacteristic->getValue();  //Always gets value as string so better sending as one aswell

    if (value.length() > 0) {
      float count;
      char tmp[value.length()];
      //Serial.print("New value: ");
      for (int i = 0; i < value.length(); i++) {
        tmp[i] = value[i];
      }

      count += atol(tmp);                          //conversion from char to float
      if (count > 99.00 && restartedInPairMode) {  //if pairmode enabled and count is a linking value (anything over 100)
        inoee.saveDis(count, NAMEADDRESS);         //as count is over 99, save it to eeprom and restart the device.
        Serial.print("Saved pair value to EEPROM, Restarting. That value is: ");
        Serial.println(count);
        delay(250);  //for debug and can actually see the print
        inoee.reboot();
      } else if (count > 99.00) {
        //do nothing

      } else {
        tickDown(count);  //proceed normally and tick down the displayed value
      }
      last_time = millis();  //reset sleep timer
    }
  }
};

class MyServerCallback : public BLEServerCallbacks {
  void onConnect(BLEServer* pserver) {
    Serial.println("onConnect");
    deviceConnected = true;
  }

  void onDisconnect(BLEServer* pserver) {

    Serial.println("onDisconnect");
    deviceConnected = false;
    delay(500);                   // give the bluetooth stack the chance to get things ready
    pServer->startAdvertising();  // restart advertising
    Serial.println("start advertising");
  }
};
//CLASSED END