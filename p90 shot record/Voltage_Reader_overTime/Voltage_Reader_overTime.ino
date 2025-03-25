static byte pin = 18;
RTC_DATA_ATTR int count = 0;  //binds it to RTC so does not clear in sleep

#define uS_TO_S_FACTOR 1000000ULL  //Conversion factor for microseconds seconds to seconds. Used by Sleep timer
#define mS_TO_S_FACTOR 1000ULL
volatile unsigned long timeV = 0;
volatile unsigned long timeEnd = 0;



void IRAM_ATTR isr() {  //esp32 specific funtion for interrupts. arduino has set pins while esp can do any. Cannot be any output during interrupt and this includes Serial output
  timeV = millis();
}


void IRAM_ATTR isr2() {  //esp32 specific funtion for interrupts. arduino has set pins while esp can do any. Cannot be any output during interrupt and this includes Serial output
    timeEnd = millis();
    timeEnd -= timeV;
    timeV = 0;
}


void setSleepwakeup() {
  gpio_wakeup_enable((gpio_num_t)pin, GPIO_INTR_HIGH_LEVEL);  //enable this pin to wake
  esp_sleep_enable_gpio_wakeup();
}

void sleepLight(long last_time, uint32_t count) {      //reference back to update values
  if (millis() > (last_time + (10 * mS_TO_S_FACTOR)))  //if last time, in seconds, is greater then 2 minutes then sleep. Aka, if idle for 0 minutes then sleep. Deepsleep has too slow a startup to use.
  {
    //detachInterrupt(digitalPinToInterrupt(PIN));

    setSleepwakeup();  //calls function to set wakeup options. Will sleep until woken by shot or timer. If timer wake then it will go in a deep sleep till woken by shot. Battery saver
    Serial.println("LightSleep");

    esp_light_sleep_start();
    Serial.println("wakeup");

    //last_time = 7;
    //count++;  //counts but does not record it
    last_time = millis();  //sets last time to ensure that it gets set. I know it is set during isr


    //attachInterrupt(digitalPinToInterrupt(PIN), isr, RISING);
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(pin, INPUT_PULLUP);
  analogReadResolution(12);


  //attachInterrupt(digitalPinToInterrupt(pin), isr, RISING);
  //attachInterrupt(digitalPinToInterrupt(pin), isr2, FALLING);
}

void inLoopAnalog() {
  float voltage = (analogRead(pin) / 4095.0) * 3.3;  //Real time voltage reading is required so this var doesn't do much


  if (voltage > 3) {   //overall check for 3v or higher
    timeV = millis();  //records start time

    while (((analogRead(pin) / 4095.0) * 3.3) > 0.6) {  //prints until lower then 3V. Loop required

      Serial.println(((analogRead(pin) / 4095.0) * 3.3));
      //time += timeFactor;
      delay(5);
    }
    //records post lower then 3v*/
  }
  if (timeEnd > 0) {
    Serial.print("time = ");
    Serial.print(timeEnd - timeV);  //print time as a difference.
    Serial.println("ms");
  }
}

void loop() {
  inLoopAnalog();
  /*if (timeEnd > 0) {
    Serial.print("time = ");
    Serial.print(timeEnd);  //print time as a difference.
    Serial.println("ms");
    timeEnd = 0;
  }*/
}
