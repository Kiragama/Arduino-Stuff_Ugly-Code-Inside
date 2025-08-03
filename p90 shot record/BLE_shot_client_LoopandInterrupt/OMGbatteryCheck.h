#include <UMS3.h>

UMS3 ums3;

void checkBattery() {
  // Get the battery voltage, corrected for the on-board voltage divider
  // Full should be around 4.2v and empty should be around 3v
  float battery = ums3.getBatteryVoltage();

  if (ums3.getVbusPresent()) {
    // If USB power is present
    if (battery < 4.0) {
      // Charging - blue
      ums3.setPixelColor(0x0000FF);
    } else {
      // Close to full - off
      ums3.setPixelColor(0x000000);
    }

    Serial.printf("Running from 5V - Battery: %fV\n", battery);
  } else {
    Serial.printf("Running from Battery: %fV\n", battery);
  }
}
