int LED01 = 10;
int LED02 = 11;
int LED03 = 12;
int Switch1 = 2;  //pin 2 will be attached to our switch
int Switch2 = 3;  //pin 3 will be attached to our switch
int Switch3 = 4;  //pin 4 will be attached to our switch
 
void setup() {
  //setup both an output AND an input on the HERO
  // pinMode(LED01, OUTPUT);
  // pinMode(LED02, OUTPUT);
  // pinMode(LED03, OUTPUT);
  // pinMode(Switch1, INPUT);
  // pinMode(Switch2, INPUT);
  // pinMode(Switch3, INPUT);

  Serial.begin(9600);
 
}
 
void loop() {
  //now within loop() we'll take actions based on the status of the input switch
Serial.println("Hello earthling!");
delay(500);
Serial.println("take me to you leader...");
delay(1000);

}           

void doDay4() {
 
  //this is a conditional test...
  if ((digitalRead(Switch1) == HIGH) && (digitalRead(Switch2) == HIGH) && (digitalRead(Switch3) == HIGH)) {
    blinkAllTheLights();
  } else {
    checkTheSwitches();
  }
}

void blinkAllTheLights() {
    turnOnTheLed(LED01);
    delay(250);
    turnOnTheLed(LED02);
    delay(250);
    turnOnTheLed(LED03);
    delay(250);
    turnOffTheLed(LED01);
    delay(250);
    turnOffTheLed(LED02);
    delay(250);
    turnOffTheLed(LED03);
    delay(250);

    turnOnTheLed(LED01);
    delay(250);
    turnOnTheLed(LED02);
    delay(250);
    turnOnTheLed(LED03);
    delay(250);
    turnOffTheLed(LED01);
    delay(250);
    turnOffTheLed(LED02);
    delay(250);
    turnOffTheLed(LED03);
    delay(250);

    turnOnTheLed(LED01);
    delay(250);
    turnOnTheLed(LED02);
    delay(250);
    turnOnTheLed(LED03);
    delay(250);
    turnOffTheLed(LED01);
    delay(250);
    turnOffTheLed(LED02);
    delay(250);
    turnOffTheLed(LED03);
    delay(250);

    turnOnTheLed(LED01);
    turnOnTheLed(LED02);
    turnOnTheLed(LED03);
    delay(250);
    turnOffTheLed(LED01);
    turnOffTheLed(LED02);
    turnOffTheLed(LED03);
    delay(250);
    turnOnTheLed(LED01);
    turnOnTheLed(LED02);
    turnOnTheLed(LED03);
    delay(250);
    turnOffTheLed(LED01);
    turnOffTheLed(LED02);
    turnOffTheLed(LED03);
    delay(250);
    turnOnTheLed(LED01);
    turnOnTheLed(LED02);
    turnOnTheLed(LED03);
    delay(250);
    turnOffTheLed(LED01);
    turnOffTheLed(LED02);
    turnOffTheLed(LED03);
    delay(250);
}

void checkTheSwitches() {
  if (digitalRead(Switch1) == HIGH){
    turnOnTheLed(LED01);
  } else {
    turnOffTheLed(LED01);
  }

  if (digitalRead(Switch2) == HIGH){
    turnOnTheLed(LED02);
  } else {
    turnOffTheLed(LED02);
 }

  if (digitalRead(Switch3) == HIGH){
    turnOnTheLed(LED03);
  } else {
    turnOffTheLed(LED03);
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
   delay(100);
   digitalWrite(ledToControl, HIGH);
   delay(100);
   digitalWrite(ledToControl, LOW);
   delay(100);
   digitalWrite(ledToControl, HIGH);
   delay(100);
}