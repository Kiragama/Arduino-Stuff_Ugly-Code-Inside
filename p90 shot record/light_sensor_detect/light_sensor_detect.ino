 // Define a variable to save the ADC value
const int pT = A0;
void setup() {
 
 Serial.begin(9600);
}
void loop() {
 int value = 0; // Read analog voltage value of pin A0, and save
 // Map analog to the 0-255 range, and works as ledPin duty cycle setting
 value = analogRead(A0);
 int convertValue = value/4;
 Serial.println(convertValue);


 delay(2);
}
