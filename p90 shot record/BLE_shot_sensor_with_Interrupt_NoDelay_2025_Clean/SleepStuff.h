#define uS_TO_S_FACTOR 1000000ULL           //Conversion factor for microseconds seconds to seconds. Used by Sleep timer
#define mS_TO_S_FACTOR 1000ULL
unsigned volatile long last_time;

RTC_DATA_ATTR volatile uint32_t count = 0;  //volatile stops the compiler from assuming the value and actually check. Required if interrupts used
#define PIN 35                              //mosi

//_______________________________________________________________________________Sleep
void setSleepwakeup(int sleepTime) {
  gpio_wakeup_enable((gpio_num_t)PIN, GPIO_INTR_HIGH_LEVEL);  //enable this pin to wake
  esp_sleep_enable_gpio_wakeup();

  //esp_sleep_enable_timer_wakeup(sleepTime * uS_TO_S_FACTOR);  //sleep time * conversion factor
}
void sleepDeep() { //light sleep offers enough of a battery life and deep takes too long to wake.
  Serial.println("DeepSleep");
  esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_TIMER);
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_35, HIGH);
  esp_deep_sleep_start();  //Nothing will happen after this. Will reset on wake
}

bool sleepLight() {
  if (millis() > (last_time + (300 * mS_TO_S_FACTOR)))  //if last time, in seconds, is greater then 5 minutes then sleep. Aka, if idle for 5 minutes then sleep. Deepsleep has too slow a startup to use.
  {
    setSleepwakeup(10);  //calls function to set wakeup options. Will sleep until woken by shot or timer. If timer wake then it will go in a deep sleep till woken by shot. Battery saver
    Serial.println("LightSleep");
    esp_light_sleep_start();
    Serial.println("wakeup");
    count++;  //counts but does not record it
    Serial.println(count);
    connected = false;  //sets connected to false as sleep does this and does not update the variable otherwise. Wakups would then think connected and reset the count.
    doConnect = true;
    last_time = millis();  //sets last time to ensure that it gets set. I know it is set during isr
    return true;           //A check to ensure the connected statement skips the first
  } else {
    return false;  //will be called on the next itteration of loop
  }
}
//_______________________________________________________________________________End of Sleep