const byte pin = 0;
//#include <Adafruit_NeoPixel.h>
int test = 0;
RTC_DATA_ATTR unsigned long last_time;

/*#define NUMPIXELS        1
Adafruit_NeoPixel pixels(NUMPIXELS, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);*/
//ICACHE_RAM_ATTR IS ESP8266
//IRAM_ATTR PUT INTERRUPT INTO ram
void IRAM_ATTR isr() {  //esp32 specific funtion for interrupts. arduino has set pins while esp can do any. Cannot be any output during interrupt and this includes Serial output
   
                        //Serial.println("bang");
                        //https://www.arduino.cc/reference/en/language/functions/external-interrupts/attachinterrupt/
                        //wifi can cause some issues with interrupts
                        //NO SERIAL COMMUNICATION IN OR AROUND INTERRUPTS AND CAUSE THEY CAN HAPPEN WHENEVER, JUST DON'T. This counts for long actions like delay.
                        //all variables in interrupts must be volatile

  //ESP32 not best for crazy fast interrpts as while it is fast, the architecture has limitations on interrupts that cause delays and can cause missed triggers

  //The sleep functionality might help with above issue and allow for wifi as it doesn't happen on processor directly. Volatile variables are lost in sleep so use RTC_DATA_ATTR instead of volatile.
  //https://www.youtube.com/watch?v=CJhWlfkf-5M&pp=ygUKIzMyOCBlc3AzMg%3D%3D

  if ((micros() > last_time+10))
  {
  test++;  //interrupts better for project as it gets around the issue with waiting for the cycle to reach the check command.
  last_time = micros();
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(pin, INPUT);
/*
   pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.setBrightness(20); // not so bright*/
  //attachInterrupt(digitalPinToInterrupt(pin), isr, RISING);
/*
  pixels.fill(0x000000);
   pixels.fill(0x00FF00);
  pixels.show();*/

  last_time = micros();
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(test);
  if (test == 10) {
   /* pixels.fill(0x000000);
    pixels.fill(0xFFC0CB);*/
  //pixels.show();
  }
}