/* This example shows how to use continuous mode in a non blocking way to take
range measurements with the VL53L0X. It is based on
vl53l0x_ContinuousRanging_Example.c from the VL53L0X API.

The range readings are in units of mm. */

#include <Wire.h>
#include <VL53L0X_mod.h>

VL53L0X_mod sensor;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  sensor.setTimeout(500);
  if (!sensor.init()) {
    Serial.println("Failed to detect and initialize sensor!");
    while (1) {}
  }

  sensor.startContinuous(2);
}

struct glo {
  int16_t once;
  uint16_t compare_value;
};

struct glo bal;
void occur_first() {
  uint16_t tmp = sensor.readRangeContinuousMillimeters();  //reads the container its in so there is a compare value. an average would be better but not worth
  bal.compare_value = tmp;
  bal.once++;
}

//first set
void loop() {
  if (bal.once != 1) {
    occur_first();
  }
  uint16_t NewMeasurement;
  if (sensor.readRangeNoBlocking(NewMeasurement)) {
    if (NewMeasurement < 88) {  //compares the difference as long as newmeasurement is less then the starting value. This is to get around the natural up and down of the read values
      Serial.println(NewMeasurement);
    }
  }
}
