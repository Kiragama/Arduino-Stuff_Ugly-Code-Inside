//gpio_num_t pin = 26;
RTC_DATA_ATTR int test = 0; //volitile but stays during sleep


void setup() {
//wakeup is not slow and startup does use a fair bit of power. Not best for long term low power
Serial.begin(9600);
pinMode(13, OUTPUT);
//print_wakeup_reason(); //serial print reason

//if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_EXT0)  

esp_sleep_enable_ext0_wakeup('26', 0); //sets wakeup to be when pin is low. 1 = HIGH

digitalWrite(13, HIGH);

esp_deep_sleep_start(); //go in low power until trigger
} //this is basically an interrupt but can last a long time and also allows for delays and Serial Output (maybe)

void loop() {
  //THis setup besically prevents loop from ever being called
}