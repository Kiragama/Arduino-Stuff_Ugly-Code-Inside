
#include <esp_sleep.h>
#include "driver/rtc_io.h"
RTC_DATA_ATTR int count = 0;
int64_t t_before_us = esp_timer_get_time();
void setup() {
  Serial.begin(115200);
  pinMode(count, INPUT_PULLDOWN);
  delay(200);
  Serial.println("wake");
  
  // put your setup code here, to run once:
  t_before_us = esp_timer_get_time();
  gpio_wakeup_enable(GPIO_NUM_0, GPIO_INTR_HIGH_LEVEL);
  //rtc_gpio_pulldown_en(GPIO_NUM_1);
  //rtc_gpio_pullup_dis(GPIO_NUM_1);
  delay(200);
  esp_sleep_enable_gpio_wakeup();
  Serial.println("Enable wakeup");
  delay(200);
  //esp_sleep_enable_ext0_wakeup(GPIO_NUM_1, 1);


  
  Serial.println("Sleep");
  esp_light_sleep_start();

}

void loop() {
  // put your main code here, to run repeatedly:
  /*
  if(digitalRead(0) == RISING){
    
  }*/
  //Serial.println("loop");

  if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_GPIO) {
    /* Waiting for the gpio inactive, or the chip will continuously trigger wakeup*/
    int64_t t_after_us = esp_timer_get_time();
    //Serial.println((int)(t_after_us / 1000), (int)((t_after_us - t_before_us) / 1000));
  }
}
