void setup() {
  // put your setup code here, to run once:

Serial.begin(9600);
}

void loop() { 

  //||Details below will need to be tailored to the specific board||
  //this assumes the input voltage is being converted from a higher voltage to 5v. if input is already <=5v then remove section involving the result variable
  // keep an eye out for noise on that pin (changing values)
  //if noise is minimal and the input set correctly, this should be accurate to the 2nd decimal point.
  float inputRef = 4.25; //this varies from device to device. on ATMEGA32U4, measure voltage of pin 24 (AVCC) on the chip and use that here.
  float in = (float)analogRead(A3);
  float voltage = in * (inputRef / 1023.0); //1023 is the steps value for a 10 bit A/D converter. inputRef is the max voltage for the AVCC pin on the ATMEGA32U4, I just measured the out voltage of pin24 on the chip. These 2 are what makes the voltage accurate as some chips might not have a 10bit A/D converter or have 4.25 as max
  //source https://www.youtube.com/watch?v=gw72g4WBz-U
  //power supply can be noisy and will affect accuracy of this reader as the basis this reader uses to measure is that input voltage (measured from AVCC by default)
  Serial.println(voltage); //debug

  //to restore the original voltage before conversion
  float result = voltage * 5; //the times value is the input voltage / output of conversion
  Serial.print("Voltage before conver is: ");//debug
  Serial.println(result);//debug
  delay(2000); //debug 
}
