int Light = 12;  // pin 12

void setup() {
  // put your setup code here, to run once:
  pinMode(Light, OUTPUT);
  digitalWrite(Light, LOW);

}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(Light, LOW);
  delay(1000);                      // wait for a second
  digitalWrite(Light, HIGH);   // turn the LED off by making the voltage LOW
  delay(250);                      // wait for a second

}
