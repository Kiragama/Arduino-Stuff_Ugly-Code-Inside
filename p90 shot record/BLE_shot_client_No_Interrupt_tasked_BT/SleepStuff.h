#include "Defines.h"

//_______________________________________________________________________________Sleep
void setSleepwakeup() {
  gpio_wakeup_enable((gpio_num_t)PIN, GPIO_INTR_HIGH_LEVEL);  //enable this pin to wake
  esp_sleep_enable_gpio_wakeup();
}

void sleepLight(long last_time, uint32_t count) {  //reference back to update values
  if (millis() > (last_time + (SLEEPTIME * mS_TO_S_FACTOR))) //if last time, in seconds, is greater then 2 minutes then sleep. Aka, if idle for 0 minutes then sleep. Deepsleep has too slow a startup to use.
    setSleepwakeup();  //calls function to set wakeup options. Will sleep until woken by shot or timer. If timer wake then it will go in a deep sleep till woken by shot. Battery saver
    Serial.println("LightSleep");
    
    esp_light_sleep_start();
    Serial.println("wakeup");
    
    //last_time = 7;
    //count++;  //counts but does not record it
    last_time = millis();  //sets last time to ensure that it gets set. I know it is set during isr

    pClient->disconnect(); //disconnecting before sleep wakes device. Device will be disconnected from server regardless.
    
  }
}
  //_______________________________________________________________________________End of Sleep