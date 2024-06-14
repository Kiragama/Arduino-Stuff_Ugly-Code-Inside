//#include <Adafruit_NeoPixel.h>
#include <driver/rtc_io.h>
//#define NUMPIXELS 1
//Adafruit_NeoPixel pixels(NUMPIXELS, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);


const byte pin = 0;
RTC_DATA_ATTR int test = 0;
RTC_DATA_ATTR unsigned long last_time;


//ICACHE_RAM_ATTR IS ESP8266
//IRAM_ATTR PUT INTERRUPT INTO ram
void IRAM_ATTR isr() {  //esp32 specific funtion for interrupts. arduino has set pins while esp can do any. Cannot be any output during interrupt and this includes Serial output

  //https://www.arduino.cc/reference/en/language/functions/external-interrupts/attachinterrupt/
  //wifi can cause some issues with interrupts
  //NO SERIAL COMMUNICATION IN OR AROUND INTERRUPTS AND CAUSE THEY CAN HAPPEN WHENEVER, JUST DON'T. This counts for long actions like delay.
  //all variables in interrupts must be volatile

  //ESP32 not best for crazy fast interrpts as while it is fast, the architecture has limitations on interrupts that cause delays and can cause missed triggers

  //The sleep functionality might help with above issue and allow for wifi as it doesn't happen on processor directly. Volatile variables are lost in sleep so use RTC_DATA_ATTR instead of volatile.
  //https://www.youtube.com/watch?v=CJhWlfkf-5M&pp=ygUKIzMyOCBlc3AzMg%3D%3D

  if ((micros() > last_time + 10))  //wait 10 microseconds between shots
  {
    test++;  //interrupts better for project as it gets around the issue with waiting for the cycle to reach the check command.
    last_time = micros();
  }
}

void setup() {
  pinMode(pin, INPUT);
  //rtc_gpio_hold_en(GPIO_NUM_9);  //hold the pinmode during sleep
  //Sleep
  esp_log_level_set("*", ESP_LOG_ERROR);  // set all components to ERROR level. Speeds up wake
  esp_deep_sleep_disable_rom_logging();   // suppress boot messages. Speeds up wake
  /*pixels.begin();                         // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.setBrightness(20);               // not so bright
  pixels.fill(0x000000);
  pixels.fill(0x00FF00);                             //green should always be on as an indicator of sleep (when off)
  pixels.show();    */                                 //no obvious slowdown from neopixel
  if(esp_sleep_get_wakeup_cause())
  {
    isr(); //if woken from sleep, then call interrupt
  }
  /* RISING - works | HIGH - works */
  //End of Sleep

  attachInterrupt(digitalPinToInterrupt(pin), isr, RISING);  //normal operation interrupt


  last_time = micros();
/*
  if ((micros() > last_time+3000000))  //if last time, in seconds, is greater then 5 minutes then sleep. Aka, if idle for 5 minutes then sleep
  {
    //put wifi/bluetooth poweroff here
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_9, RISING);  //sleep until shot fired
    esp_deep_sleep_start();
  }*/
}

void loop() {
  // put your main code here, to run repeatedly:
  //Serial.println(test);
  if (test == 10) {
    /*pixels.fill(0x000000);
    pixels.fill(0xFFC0CB);
    pixels.show();*/
    Serial.println(test);
  }
  Serial.println(test);
}