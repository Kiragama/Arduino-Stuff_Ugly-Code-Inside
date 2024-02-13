/***

    eeprom_get example.

    This shows how to use the EEPROM.get() method.

    To pre-set the EEPROM data, run the example sketch eeprom_put.

    This sketch will run without it, however, the values shown

    will be shown from what ever is already on the EEPROM.

    This may cause the serial object to print out a large string

    of garbage if there is no null character inside one of the strings

    loaded.

    Written by Christopher Andrews 2015

    Released under MIT licence.

***/

#include <EEPROM.h>

void setup() {

  int eeAddress = 0; //EEPROM address to start reading from

  Serial.begin(9600);

  while (!Serial) {

    ; // wait for serial port to connect. Needed for native USB port only

  }

  eeAddress = sizeof(float); //Move address to the next byte after float 'f'.

  float test; //Variable to store custom object read from EEPROM.

  EEPROM.get(eeAddress, test); //data must be a float

  Serial.println("Read custom object from EEPROM: ");

  Serial.println(test);

}

void loop() {

  /* Empty loop */
}