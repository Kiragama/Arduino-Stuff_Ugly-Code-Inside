#include <Adafruit_NeoPixel.h>
#include <driver/rtc_io.h>
#define NUMPIXELS 1
Adafruit_NeoPixel pixels(NUMPIXELS, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);

RTC_DATA_ATTR int count = 0;  //volitile but stays during sleep

//DOUBLE TAP DOUBLE TAP THE RESET BUTTON TO END THE LOOP
//#define THRESHOLD   5000   /* Lower the value, more the sensitivity */ //uses the touch pins to wake

//touch_pad_t touchPin;
//fast wakeup source: https://esp32.com/viewtopic.php?t=30954
void setup() {
  //Sleep
  esp_log_level_set("*", ESP_LOG_ERROR);  // set all components to ERROR level. Speeds up wake
  esp_deep_sleep_disable_rom_logging();   // suppress boot messages
  pixels.begin();            // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.setBrightness(20);  // not so bright
  pixels.fill(0x000000);
  pixels.fill(0xFFC0CB);  //pink
  pixels.show(); //no obvious slowdown from neopixel
  delay(2000);

  //End of Trigger Actions --

  //Sleep reason
  //touchSleepWakeUpEnable(T9,THRESHOLD); //T9 is GPIO 9 (A2) //uses the touch pins to wake
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_9, RISING);  //shots at speed work reliably after device restart but after sleep, can be weird sometimes and record the first show. But not anymore

  /* 
RISING - works 
HIGH - works
*/
  //Sleep After
  esp_deep_sleep_start();  //go in low power until trigger
}  //this is basically an interrupt but can last a long time and also allows for delays and Serial Output (maybe)

void loop() {
  //THis setup besically prevents loop from ever being called
}


//https://learn.adafruit.com/adafruit-qt-py-esp32-s2/pinouts - qt py