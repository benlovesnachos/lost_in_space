int LED01 = 10; // green
int LED02 = 11; // blue
int LED03 = 12; // red
int Switch1 = 2;  //pin 2 will be attached to our switch
int Switch2 = 3;  //pin 3 will be attached to our switch
int Switch3 = 4;  //pin 4 will be attached to our switch

// Day 6 code...
 
int sensorPin = A0;   // select the *analog zero* input pin for probing the photoresistor
int onboardLED = LED_BUILTIN;  // select the pin for the HERO's built-in LED
int sensorValue = 0;  // variable that we'll use to store the value reading from the sensor
 
// Day 7
unsigned int batteryCapacity = 50000;
unsigned int batteryLevel = 0;
unsigned int ticks = 0;
unsigned int wait = 100;
double PercentFull;
  
void setup() {
  //setup both an output AND an input on the HERO
  pinMode(LED01, OUTPUT);
  pinMode(LED02, OUTPUT);
  pinMode(LED03, OUTPUT);
  pinMode(Switch1, INPUT);
  pinMode(Switch2, INPUT);
  pinMode(Switch3, INPUT);

  Serial.begin(9600);
  pinMode(onboardLED, OUTPUT); 
}


void loop() {
   // Pulse the builtin LED for a time determined by the sensor
  sensorValue = analogRead(sensorPin);
  digitalWrite(onboardLED, HIGH);
  delay(sensorValue);
  digitalWrite(onboardLED, LOW);
  delay(sensorValue);
 
   // Read the analog sensor value and send it to serial
  Serial.println(sensorValue);
  delay(sensorValue);

  // displayChargeStatus();
  doBatteryCharging();
}



void PrintBatteryPercentage() {
  // print the elasped time
  Serial.print(ticks);
  Serial.print(" ms    charge at ");
 
  //  convert the integers to decimal numbers, divide them and print...
  PercentFull=100*((double)batteryLevel / (double)batteryCapacity);
  Serial.print(PercentFull);
 
  // print a percent character and line return...
  Serial.println("%");
}
 

void doBatteryCharging() {
  sensorValue = analogRead(sensorPin); // read the value from the sensor:
  batteryLevel += sensorValue;
  ticks += wait;
 
  if(batteryLevel >= batteryCapacity) {
    Serial.print(ticks);
    Serial.print(" ms     ");
    Serial.println("FULLY CHARGED");
    batteryLevel = batteryCapacity; // to prevent integer from continuing to increase
    ticks = 0;
    delay(20000);      // long pause
  }
  else {
    PrintBatteryPercentage();
  }
 
  delay(wait);
}
 

void testSerialOutput() {
  Serial.println("Hello earthling!");
  delay(500);
  Serial.println("take me to you leader...");
  delay(1000);
}           



void displayChargeStatus() {
  // sensorValue = analogRead(sensorPin);
  digitalWrite(LED02, HIGH);
  delay(sensorValue);
  digitalWrite(LED02, LOW);
  delay(sensorValue);

  // Serial.println(sensorValue);
  // delay(sensorValue);

  if((sensorValue) > 45){
    Serial.println("charging...");    
    digitalWrite(LED01, HIGH);
    digitalWrite(LED03, LOW);
    // delay(sensorValue);
  } else if((sensorValue) <45){
    Serial.println("discharging...");    
    digitalWrite(LED03, HIGH);
    digitalWrite(LED01, LOW);
    // delay(sensorValue);
  }
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