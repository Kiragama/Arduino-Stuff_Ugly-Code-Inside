/*
 * LucidGloves Firmware Version 4
 * Author: Lucas_VRTech - LucidVR
 * lucidvrtech.com
 */

#include "ConfigUtils.h"
#include "AdvancedConfig.h"


//This is the configuration file, main structure in _main.ino
//CONFIGURATION SETTINGS:
#define ESP32S3 // Hack to workaround pin32 issue. Uncomment if you have an ESP32S3.
//#define NEOPIXEL //Is the DEBUG_LED a WS2812? 
#define COMMUNICATION COMM_SERIAL //Which communication protocol to use. Options are: COMM_SERIAL (usb), COMM_BTSERIAL (bluetooth), COMM_BLESERIAL (BLE Serial, using Nordic UART Service)
//serial over USB
  #define SERIAL_BAUD_RATE 115200
  
//serial over Bluetooth
  #define BTSERIAL_DEVICE_NAME "lucidgloves-left"

//ANALOG INPUT CONFIG
#define USING_SPLAY false //whether or not your glove tracks splay. - tracks the side to side "wag" of fingers. Requires 5 more inputs.
#define USING_MULTIPLEXER false //Whether or not you are using a multiplexer for inputs
#define FLIP_FLEXION  true  //Flip values from potentiometers (for fingers!) if they are backwards
#define FLIP_SPLAY false //Flip values for splay


//Gesture enables, make false to use button override
#define TRIGGER_GESTURE true
#define GRAB_GESTURE    true
#define PINCH_GESTURE   true


//BUTTON INVERT
//If a button registers as pressed when not and vice versa (eg. using normally-closed switches),
//you can invert their behaviour here by setting their line to true.
//If unsure, set to false
#define INVERT_A false
#define INVERT_B false
#define INVERT_JOY false
#define INVERT_MENU true
#define INVERT_CALIB false
//These only apply with gesture button override:
#define INVERT_TRIGGER true
#define INVERT_GRAB false
#define INVERT_PINCH false


//joystick configuration
#define JOYSTICK_BLANK false //make true if not using the joystick
#define JOY_FLIP_X false
#define JOY_FLIP_Y false
#define JOYSTICK_DEADZONE 10 //deadzone in the joystick to prevent drift (in percent)

#define NO_THUMB false //If for some reason you don't want to track the thumb

#define USING_CALIB_PIN true //When PIN_CALIB is shorted (or it's button pushed) it will reset calibration if this is on.

#define USING_FORCE_FEEDBACK false //Force feedback haptics allow you to feel the solid objects you hold
#define FLIP_FORCE_FEEDBACK false
#define SERVO_SCALING false //dynamic scaling of servo motors


  //(This configuration is for ESP32 DOIT V1 so make sure to change if you're on another board)
  //To use a pin on the multiplexer, use MUX(pin). So for example pin 15 on a mux would be MUX(15).
  #define PIN_PINKY     6 //E These 5 are for flexion
  #define PIN_RING      10 //D
  #define PIN_MIDDLE    18 //C
  #define PIN_INDEX     15 //B
  #define PIN_THUMB     2  //A
  #define PIN_JOY_X     13 //F
  #define PIN_JOY_Y     14 //G
  #define PIN_JOY_BTN   35
  #define PIN_A_BTN     21 //j
  #define PIN_B_BTN     3  //k
  #define PIN_TRIG_BTN  46 //unused if gesture set
  #define PIN_GRAB_BTN  48 //unused if gesture set
  #define PIN_PNCH_BTN  -1//47 was switched with calibration to fix an issue where pin 36 causes boot loops (on multiple boards)  | //unused if gesture set
  #define PIN_CALIB     47 //36button for recalibration (You can set this to GPIO0 to use the BOOT button, but only when using Bluetooth.)
  #define DEBUG_LED -1
  #define PIN_PINKY_MOTOR     42  //used for force feedback
  #define PIN_RING_MOTOR      41 //^
  #define PIN_MIDDLE_MOTOR    40 //^
  #define PIN_INDEX_MOTOR     39 //^
  #define PIN_THUMB_MOTOR     38 //^
  #define PIN_MENU_BTN        45

  //Splay pins. Only used for splay tracking gloves. Use MUX(pin) if you are using a multiplexer for it.
  #define PIN_PINKY_SPLAY  4
  #define PIN_RING_SPLAY   12
  #define PIN_MIDDLE_SPLAY 9
  #define PIN_INDEX_SPLAY  17
  #define PIN_THUMB_SPLAY  7  

  //Select pins for multiplexers, set as needed if using a mux. You can add or remove pins as needed depending on how many select pins your mux needs.
  #define PINS_MUX_SELECT     -1,  /*S0 pin*/ \
                              -1,  /*S1 pin*/ \
                              -1,  /*S2 pin*/ \
                              -1   /*S3 pin (if your mux is 3-bit like 74HC4051 then you can remove this line and the backslash before it.)*/
  
  #define MUX_INPUT -1  //the input or SIG pin of the multiplexer. This can't be a mux pin.

  //Signal mixing for finger values. Options are: MIXING_NONE, MIXING_SINCOS
  //For double rotary hall effect sensors use MIXING_SINCOS. For potentiometers use MIXING_NONE.
  #define FLEXION_MIXING MIXING_SINCOS
    //Secondary analog pins for mixing flexion values. Only used by MIXING_SINCOS. Use MUX(pin) if you are using a multiplexer for it.
    #define PIN_PINKY_SECOND     5 
    #define PIN_RING_SECOND      11
    #define PIN_MIDDLE_SECOND    8
    #define PIN_INDEX_SECOND     16
    #define PIN_THUMB_SECOND     1
  