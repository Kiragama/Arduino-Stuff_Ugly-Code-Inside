/*
  Fading

  This example shows how to fade an LED using the analogWrite() function.

  The circuit:
  - LED attached from digital pin 9 to ground.

  created 1 Nov 2008
  by David A. Mellis
  modified 30 Aug 2011
  by Tom Igoe

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/Fading
*/

int ledPin = 3;    
int ledPin2 = 5;
int ledPin3 = 6;
int ledPin4 = 9;
int ledPin5 = 10;
int ledOn = 7;
int ledOn2 = 8;

//needles are power by vcc
int cycleLeds;
void setup() {
  // nothing happens in setup
  pinMode(ledOn, OUTPUT);
  pinMode(ledOn2, OUTPUT);
}

void loop() {
  digitalWrite(ledOn, HIGH);
  digitalWrite(ledOn2, HIGH);
  // fade in from min to max in increments of 5 points:
  for (cycleLeds = ledPin2; cycleLeds <= ledPin5; cycleLeds++)
  {
  if(cycleLeds != 4 && cycleLeds != 7 && cycleLeds != 8)
  {  
   for (int fadeValue = 0 ; fadeValue <= 255; fadeValue += 50) {
     // sets the value (range from 0 to 255):
     analogWrite(cycleLeds, fadeValue);
     
     delay(10);
    }
  }

  if(cycleLeds != 4 && cycleLeds != 7 && cycleLeds != 8)
  { 
  // fade out from max to min in increments of 5 points:
    for (int fadeValue = 255 ; fadeValue >= 0; fadeValue -= 50) {
      // sets the value (range from 0 to 255):
      analogWrite(cycleLeds, fadeValue);
      // wait for 30 milliseconds to see the dimming effect
      delay(10);
    }
  }
  }
}
