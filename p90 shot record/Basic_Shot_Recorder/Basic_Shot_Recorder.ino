
void setup() {
  // put your setup code here, to run once:

  pinMode(12, OUTPUT);

  //below is an interupt method that relies on interrupt pins on the processor. PCINT0 in this case, which goes to pin 8 on the nano and uno (same processor)
  //source:
  //https://electronoobs.com/eng_arduino_tut166_code1.php
  //https://www.electrosoftcloud.com/en/pcint-interrupts-on-arduino/
  //
  PCICR |= (1 << PCIE0);    //enable PCMSK0 scan  
  PCMSK0 |= (1 << PCINT0);  //Set pin D8 trigger an interrupt on state change. 
  
}

//another interrupt method and more common: https://www.arduino.cc/reference/en/language/functions/external-interrupts/attachinterrupt/
//actually completly differnt methods

void loop() {

}
//code extracted from electronoobs. 
ISR(PCINT0_vect){ //pin change on PCINT0
  if(PINB & B00000001){  //I think this triggers if input and PCINT0, or binary 1 (B00000001), match. Not really sure 
    digitalWrite(12, HIGH);
  }
}//This code works at low speed with IR, LED/LDRs but not at high speed. Created from a chrono project for a fucking coil gun so should be fast enough.

//Try with unfocussed laser light and an array of LDRs or strong IR light with alot of IR recievers. 
//Just then need to calculate the slight change in output. Easy for LDR as can run them in series and use the module to get HIGH

//See if the chronos IR recievers are in series aswell