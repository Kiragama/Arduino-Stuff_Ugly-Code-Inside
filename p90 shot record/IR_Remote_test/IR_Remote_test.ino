#define DECODE_NEC       //Must be first
#define ARDUINO_AVR_PROMICRO

#include <Arduino.h>
#include "PinDefinitionsAndMore.h"
#include <IRremote.hpp> // include the library

const int r_Power = 0x45;
const int r_Menu = 0x47;
const int r_Test = 0x44;
const int r_Up = 0x40;
const int r_Back = 0x43;
const int r_Left = 0x7;
const int r_Play = 0x15;
const int r_Right = 0x9;
const int r_Zero = 0x16;
const int r_One = 0xC;
const int r_Two = 0x18;
const int r_Three = 0x5E;
const int r_Four = 0x8;
const int r_Five = 0x1C;
const int r_Six = 0x5A;
const int r_Seven = 0x42;
const int r_Eight = 0x52;
const int r_Nine = 0x4A;
const int r_Clear = 0xD;
const int r_Down = 0x19;

/*
 * This include defines the actual pin number for pins like IR_RECEIVE_PIN, IR_SEND_PIN for many different boards and architectures
 */


void setup() {
    Serial.begin(115200);
    // Pin 2 and is hard coded in library
    IrReceiver.begin(A2);
}

void loop() {
    /*
    IR SECTION
     * Check if received data is available and if yes, try to decode it.
     * Decoded result is in the IrReceiver.decodedIRData structure.
     * Current Remote Key hex (missing the 0x ofc, e.g. 0x5E):
     */
    if (IrReceiver.decode()) {
        //IrReceiver.printIRSendUsage(&Serial); //print whole results
        //Serial.println(IrReceiver.decodedIRData.command, HEX); //print command in hex
        
        switch(IrReceiver.decodedIRData.command){
          case r_Up: //up
          Serial.println("up");
          break;
          
          case r_Down: //down
          Serial.println("down");
          break;

          case r_Left: //left
          Serial.println("left");
          break;

          case r_Right: //right
          Serial.println("right");
          break;

          case r_Zero: //0
          Serial.println("0");
          break;

          case r_One: //1
          Serial.println("1");
          break;

          case r_Two: //2
          Serial.println("2");
          break;

          case r_Three://3
          Serial.println("3");
          break;

          case r_Four://4
          Serial.println("4");
          break;

          case r_Five://5
          Serial.println("5");
          break;

          case r_Six://6
          Serial.println("6");
          break;

          case r_Seven://7
          Serial.println("7");
          break;

          case r_Eight://8
          Serial.println("8");
          break;

          case r_Nine://9
          Serial.println("9");
          break;

          case r_Clear://Clear
          Serial.println("Clear");
          break;

          case r_Power://Power
          Serial.println("Power");
          break;

          case r_Menu://Menu
          Serial.println("Menu");
          break;

          case r_Back://Back
          Serial.println("Back");
          break;

          case r_Test://Test
          Serial.println("Test");
          break;

          case r_Play://Play
          Serial.println("Play");
          break;

          default:
          Serial.println("Bad Read");
          break;
        }
        delay(100);
        IrReceiver.resume(); // Enable receiving of the next value
    }
}
