int out = 0;
const int pinOu = 15;

void setup() {
  // put your setup code here, to run once:
  pinMode(pinOu, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
analogWrite(pinOu, 100000);
  //digitalWrite(pinOu, HIGH);
}
