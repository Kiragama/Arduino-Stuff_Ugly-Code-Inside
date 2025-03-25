#pragma once

void IRAM_ATTR isr() {                //esp32 specific funtion for interrupts. arduino has set pins while esp can do any. Cannot be any output during interrupt and this includes Serial output
  if (millis() > (last_time + INTERRUPTDELAYFACTOR) && !locked)  //4.24 wait | EXAMPLE: 1 millisecond between shots. At 350 FPS, the bb will travel 106mm in a millisecond. (350/1000 (1000  nds in a second) = 0.35 feet. 0.35 is 106mm)
  {
    isrcount = 1;  //count increments so if multiple shots trigger before upload, it is still counted
    count += 1;
    last_time = millis();
    //locked = true;
  }
  
}  // NO SERIAL!!!!!!!!!!!!


void IRAM_ATTR isrFall() {                //so it has to wait for the fall
  if (isrcount < 1 && millis() > (last_time + INTERRUPTDELAYFACTOR))  //4.24 wait | EXAMPLE: 1 millisecond between shots. At 350 FPS, the bb will travel 106mm in a millisecond. (350/1000 (1000  nds in a second) = 0.35 feet. 0.35 is 106mm)
  {
    count +=1;  //count increments so if multiple shots trigger before upload, it is still counted
    last_time = millis();
  }
  isrcount = 0;
}  // NO SERIAL!!!!!!!!!!!!

void attachI() { 
  attachInterrupt(digitalPinToInterrupt(PIN), isr, RISING);       //normal operation interrupt
  attachInterrupt(digitalPinToInterrupt(PIN), isrFall, FALLING);  //normal operation interrupt
} //There are methods called interrupt() and NoInterrupt() but when tested cause Serial to get stuck. it might be a mistake on my part tho.


