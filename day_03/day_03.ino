int LED = 12;
int Switch1 = 2;  //pin 2 will be attached to our switch
 
void setup() {
//setup both an output AND an input on the HERO
 pinMode(LED, OUTPUT);
 pinMode(Switch1, INPUT);
 
}
 
void loop() {
 //now within loop() we'll take actions based on the status of the input switch
 
//this is a conditional test...
 
  if (digitalRead(Switch1) == HIGH){
    // blinkTheLed(LED);
    turnOnTheLed(LED);
  } else {
    turnOffTheLed(LED);
 }
}

void turnOffTheLed(int ledToControl) {
  digitalWrite(ledToControl, LOW); // turn LED OFF
}

void turnOnTheLed(int ledToControl) {
   digitalWrite(ledToControl, HIGH);
}

void blinkTheLed(int ledToControl) {
   digitalWrite(ledToControl, LOW);
   delay(1000);
   digitalWrite(ledToControl, HIGH);
   delay(100);
   digitalWrite(ledToControl, LOW);
   delay(100);
   digitalWrite(ledToControl, HIGH);
   delay(100);
}