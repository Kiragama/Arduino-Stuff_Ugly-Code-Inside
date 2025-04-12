unsigned volatile static long last_time;
int count = 0;
int lastCount = 0;
void IRAM_ATTR isr() {                //esp32 specific funtion for interrupts. arduino has set pins while esp can do any. Cannot be any output during interrupt and this includes Serial output
 if (millis() > (last_time + 1000))  
  {
    count++;
    //Serial.println("Trigger");
    last_time = millis();
  } //Interrupt that is held will constantly trigger. Even on rising
}  // touch does read but 3v

void IRAM_ATTR isr2() {                //esp32 specific funtion for interrupts. arduino has set pins while esp can do any. Cannot be any output during interrupt and this includes Serial output
 if (millis() > (last_time + 500))  
  {
    Serial.println("Trigger");
    last_time = millis();
  }
 
 
}  // TOUCHING THE PIN WILL TRIGGER CONSTANTLY. APPLYING 3V WILL TRIGGER ONLY UNTIL RELEASED. Theory is that the floating voltage rapidly varies, triggering, while 3v doesn't.
//touching solved by 10K to GND

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(35, INPUT);
  last_time = millis();
  attachInterrupt(digitalPinToInterrupt(35), isr2, HIGH);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  if (millis() > (last_time + 500) && digitalRead(35)){
    Serial.println("Trigger");
    last_time = millis();
  }


  
}
