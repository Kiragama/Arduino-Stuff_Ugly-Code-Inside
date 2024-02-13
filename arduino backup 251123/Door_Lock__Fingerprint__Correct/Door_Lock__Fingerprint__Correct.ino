/***************************************************
  This is an example sketch for our optical Fingerprint sensor

  Designed specifically to work with the Adafruit BMP085 Breakout
  ----> http://www.adafruit.com/products/751

  These displays use TTL Serial to communicate, 2 pins are required to
  interface
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ****************************************************/


#include <Adafruit_Fingerprint.h>
#include<Keypad.h>


//-------------------------------KeyPad------------------------------
const int SIZE = 5; //size of passcode and used for related functions
const int keySIZE = 4;
const byte keyInReset = 0;

const char keys[keySIZE][keySIZE] {
  {'1', '2', '3', 'A'},//Row
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
  //Col
};//2D array for buttons

const byte colPins[4] = {7, 6, 5, 4}; //has to be backwards cause pins are forwards (pins could be changed but this easier)
const byte rowPins[4] = {12, 11, 10, 9}; //pinout for rows/ cols

const char code[SIZE] = {'6', '1', '6', '2', '#'};
//passcode

Keypad myKeypad = Keypad(makeKeymap(keys), rowPins, colPins, 4, 4);

//------------------------------------------------------------------------------


//-----------------------------------Fingerprint----------------------------------
#if (defined(__AVR__) || defined(ESP8266)) && !defined(__AVR_ATmega2560__)
// For UNO and others without hardware serial, we must use software serial...
// pin #2 is IN from sensor (GREEN wire)
// pin #3 is OUT from arduino  (WHITE wire)
// Set up the serial port to use softwareserial..
SoftwareSerial mySerial(2, 3);

#else
// On Leonardo/M0/etc, others with hardware serial, use hardware serial!
// #0 is green wire, #1 is white
#define mySerial Serial1

#endif


Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

const int lockPin = 8; //pin that unlocks the door

//-----------------------------------------------------------------------------------



void setup()
{
  Serial.begin(9600);
  while (!Serial);  // For Yun/Leo/Micro/Zero/...
  delay(100);
  Serial.println("\n\nAdafruit finger detect test");

  // set the data rate for the sensor serial port
  finger.begin(57600);
  delay(5);
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) {
      delay(1);
    }
  }

  Serial.println(F("Reading sensor parameters"));
  finger.getParameters();
  Serial.print(F("Status: 0x")); Serial.println(finger.status_reg, HEX);
  Serial.print(F("Sys ID: 0x")); Serial.println(finger.system_id, HEX);
  Serial.print(F("Capacity: ")); Serial.println(finger.capacity);
  Serial.print(F("Security level: ")); Serial.println(finger.security_level);
  Serial.print(F("Device address: ")); Serial.println(finger.device_addr, HEX);
  Serial.print(F("Packet len: ")); Serial.println(finger.packet_len);
  Serial.print(F("Baud rate: ")); Serial.println(finger.baud_rate);

  finger.getTemplateCount();

  if (finger.templateCount == 0) {
    Serial.print("Sensor doesn't contain any fingerprint data. Please run the 'enroll' example.");
  }
  else {
    Serial.println("Waiting for valid finger...");
    Serial.print("Sensor contains "); Serial.print(finger.templateCount); Serial.println(" templates");
  }

  pinMode(lockPin, OUTPUT);
  digitalWrite(lockPin, HIGH);


  setPurple(); //led control, same for red and blue
}

void loop()                     // run over and over again
{
  //my edits
  //leds RED breathing

  //finger.LEDcontrol(FINGERPRINT_LED_OFF, 0, FINGERPRINT_LED_RED);
  //finger.LEDcontrol(FINGERPRINT_LED_ON, 0, FINGERPRINT_LED_RED);

  keyPadOperation();
  //end my edits
  getFingerprintID();
  //delay(1000);
  //don't ned to run this at full speed.
}

void setBlue()
{
  finger.LEDcontrol(FINGERPRINT_LED_ON, 0, FINGERPRINT_LED_BLUE);
}

void setRed()
{
  finger.LEDcontrol(FINGERPRINT_LED_ON, 0, FINGERPRINT_LED_RED);
  delay(250);
  setPurple();
}

void setPurple()
{
  finger.LEDcontrol(FINGERPRINT_LED_BREATHING, 250, FINGERPRINT_LED_PURPLE);
}
uint8_t getFingerprintID() {
  //changes LED based on good or bad result (blue red)


  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("No finger detected");

      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      setRed();
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      setRed();
      return p;
    default:
      Serial.println("Unknown error");
      setRed();
      return p;
  }

  // OK success!

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      setRed();
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      setRed();
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      setRed();
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      setRed();
      return p;
    default:
      Serial.println("Unknown error");
      setRed();
      return p;
  }

  // OK converted!
  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
    setBlue();
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    setRed();
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);

  openDoor();

  return finger.fingerID;
}

void openDoor()
{ //turns pin off then on 20 seconds later
  
  digitalWrite(lockPin, LOW);
  delay(20000);
  digitalWrite(lockPin, HIGH);
  setPurple();
}

// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;

  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  return finger.fingerID;
}



//KEYPAD SHIT
void keyPadOperation()
{
  static byte keyInNum = keyInReset;
  static char input[SIZE]; //saves content to memory and keeps after each loop

  char kPressed = myKeypad.getKey();

  if (kPressed)
  {
    input[keyInNum++] = kPressed; //saves pressed keys to input and records total number of presses
    Serial.println(keyInNum); //troubleshooting

    switch (kPressed)
    {
      case '*': //cancelled
        keyInNum = keyInReset; //because it is incremented just before saving to array (if 0 then the first num would be at 1 in array)
        Serial.println("*");
        return; //no break cause it restarts loop

      case '#': //confirm input
        Serial.println(kPressed); //troubleshooting
        if (keyInNum == SIZE) //checks size and that # has been entered to confirm
        {

          Serial.println("SIZE Pass"); //troubleshooting
          bool correct = true; //sets true and waits for possible false
          for (int i = 0; i < SIZE; i++)
          {
            Serial.println(i); //troubleshooting, says at which point the passcode fails
            if (input[i] != code[i])
            {
              Serial.println("false triggered");
              correct = false; //it checks for wrong because it check each character, meaning it would only set false once (when invalid key entered). if it was set to check true then it would trigger unlock 4 times (more efficient than putting else)
              break; //stops the for loop when one character wrong
            }
          }
          if (correct)
          {
            correctInput();
          }
          else
          {
            incorrectInput();
          }
        }//is more secure to reset input
        keyInNum = keyInReset; //resets it. input shouldn't need reset cause it overwritten not incremented
        break;
    }
  }
}

void correctInput()
{
  Serial.println("correct");
  setBlue();//confirm that open
  openDoor();
}

void incorrectInput()
{
  Serial.println("incorrect");
}
