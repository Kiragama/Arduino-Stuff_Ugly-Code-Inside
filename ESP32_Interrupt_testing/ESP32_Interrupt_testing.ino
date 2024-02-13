const byte pin = 35;
volatile int test = 0;

portMUX_TYPE synch = portMUX_INITIALIZER_UNLOCKED; //initialize interrupts

//ICACHE_RAM_ATTR IS ESP8266
//IRAM_ATTR PUT INTERRUPT INTO ram
void IRAM_ATTR isr() {  //esp32 specific funtion for interrupts. arduino has set pins while esp can do any. Cannot be any output during interrupt and this includes Serial output
   portENTER_CRITICAL(&synch); //must be at the start and used for syncing with program and other interrupts
                        //Serial.println("bang");
                        //https://www.arduino.cc/reference/en/language/functions/external-interrupts/attachinterrupt/
                        //wifi can cause some issues with interrupts
                        //NO SERIAL COMMUNICATION IN OR AROUND INTERRUPTS AND CAUSE THEY CAN HAPPEN WHENEVER, JUST DON'T. This counts for long actions like delay.
                        //all variables in interrupts must be volatile

  //ESP32 not best for crazy fast interrpts as while it is fast, the architecture has limitations on interrupts that cause delays and can cause missed triggers

  //The sleep functionality might help with above issue and allow for wifi as it doesn't happen on processor directly. Volatile variables are lost in sleep so use RTC_DATA_ATTR instead of volatile.
  //https://www.youtube.com/watch?v=CJhWlfkf-5M&pp=ygUKIzMyOCBlc3AzMg%3D%3D
  test = 1;  //interrupts better for project as it gets around the issue with waiting for the cycle to reach the check command.
portEXIT_CRITICAL(&synch); //must be at the end
}

void setup() {
  // put your setup code here, to run once:
  pinMode(pin, INPUT);
  pinMode(6, OUTPUT);
  //Serial.begin(9600);
  attachInterrupt(digitalPinToInterrupt(pin), isr, CHANGE);
}

void loop() {
  // put your main code here, to run repeatedly:
  //Serial.println(test);
  if (test == 1) {
    digitalWrite(6, HIGH);
/*
    good alternative to delay that works with interrupts is:
    lastEntry = millis();
    while(millis() <lastEntry + 1000) //while the current time is 1 second after the last recording of it, trigger code
    {
      all delay code does here
    }*/
  }
}