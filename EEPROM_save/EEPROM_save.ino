/***

    eeprom_put example.

    This shows how to use the EEPROM.put() method.

    Also, this sketch will pre-set the EEPROM data for the

    example sketch eeprom_get.

    Note, unlike the single byte version EEPROM.write(),

    the put method will use update semantics. As in a byte

    will only be written to the EEPROM if the data is actually

    different.

    Written by Christopher Andrews 2015

    Released under MIT licence.

***/

#include <EEPROM.h>
#define EEPROM_SIZE 10 //bytes

void setup() {

  Serial.begin(9600);
  pinMode(10, INPUT);
  EEPROM.begin(EEPROM_SIZE);
  Serial.println("Notdone1");
  int eeAddress = 0;   //Location we want the data to be put.

  while (!Serial) {
    digitalWrite(10, HIGH);
    ; // wait for serial port to connect. Needed for native USB port only

  }


  Serial.println("Notdone");
  float test = 69.0;
  eeAddress += sizeof(float); //Move address to the next byte after float 'f'.
  EEPROM.writeFloat(eeAddress, test); //data must be a float

  EEPROM.commit();
  Serial.println("done");
}

void loop() {
  Serial.println("Loop");
  delay(1000);
  /* Empty loop */
}