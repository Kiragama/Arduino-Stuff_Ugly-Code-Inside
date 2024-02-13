void setup() {
  // put your setup code here, to run once:
  //pinMode(A0, INPUT);
  Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:
  int val = analogRead(A3);
  if (val < 1010)
  {
    //Serial.println("bang");
    Serial.println(val);
  }
//delay(20);
}
