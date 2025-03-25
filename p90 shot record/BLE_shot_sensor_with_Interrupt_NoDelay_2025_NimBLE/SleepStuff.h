#define uS_TO_S_FACTOR 1000000ULL  //Conversion factor for microseconds seconds to seconds. Used by Sleep timer
#define mS_TO_S_FACTOR 1000ULL



//mosi
#ifdef PIN
//nothing
#else
#define PIN 35
#endif
//_______________________________________________________________________________Sleep
void setSleepwakeup(int sleepTime) {
  gpio_wakeup_enable((gpio_num_t)PIN, GPIO_INTR_HIGH_LEVEL);  //enable this pin to wake
  esp_sleep_enable_gpio_wakeup();
}

void sleepLight(volatile uint32_t& last_time, volatile uint32_t& count) {  //reference back to update values
  if (millis() > (last_time + (300 * mS_TO_S_FACTOR)))                      //300if last time, in seconds, is greater then 5 minutes then sleep. Aka, if idle for 5 minutes then sleep. Deepsleep has too slow a startup to use.
  {
    setSleepwakeup(10);  //calls function to set wakeup options. Will sleep until woken by shot or timer. If timer wake then it will go in a deep sleep till woken by shot. Battery saver
    Serial.println("LightSleep");
    esp_light_sleep_start();
    Serial.println("wakeup");
    pClient->disconnect(); //disconnecting before sleep wakes device. Device will be disconnected from server regardless.
    count++;  //counts but does not record it
    last_time = millis();  //sets last time to ensure that it gets set. I know it is set during isr
  }
  count *= count;
  last_time *= last_time; //dereferencing.
}
  //_______________________________________________________________________________End of Sleep