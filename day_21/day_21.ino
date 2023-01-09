// *****************************************************************************************
// This is a simple countdown timer, the idea was on some of the comments in the challenge.
// *****************************************************************************************
#include <TM1637Display.h>
#include <U8glib.h>
 
// Rotary Encoder Inputs
#define CLK2 2
#define DT2 3
#define SWITCH 4
 
// Define the display connection pins:
#define CLK 6
#define DIO 5

#define SECRET1 23
#define SECRET2 353
#define SECRET3 1688

// Create display object of type TM1637Display:
TM1637Display OurDisplay = TM1637Display(CLK, DIO);

U8GLIB_SH1106_128X64  My_u8g_Panel(U8G_I2C_OPT_NONE); // I2C / TWI
 
// Create array that turns all segments on:
const uint8_t data[] = {0xff, 0xff, 0xff, 0xff};
 
// Create array that turns all segments off:
const uint8_t blank[] = {0x00, 0x00, 0x00, 0x00};
 
// You can set the individual segments per digit to spell words or create other symbols:
const uint8_t done[] = {
  SEG_B | SEG_C | SEG_D | SEG_E | SEG_G,           // d
  SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,   // O
  SEG_C | SEG_E | SEG_G,                           // n
  SEG_A | SEG_D | SEG_E | SEG_F | SEG_G            // E
};
 
int counter = 0;    // keep a running tally of steps...
int currentStateCLK;
int lastStateCLK;
String currentDir ="";
int buzzer = 10;  // pin 10 drives the buzzer
unsigned long lastButtonPress = 0;
unsigned long lastScreenRefresh = 0;
bool showScreenA = true;
int toneFreq = 23;

void setup() {
 
  // Set encoder pins as inputs
  pinMode(CLK2,INPUT);
  pinMode(DT2,INPUT);
  pinMode(SWITCH,INPUT_PULLUP);
 
  // Setup Serial Monitor
  Serial.begin(9600);

  // flip screen, if required
  // My_u8g_Panel.setRot180();
  // assign default color value
  My_u8g_Panel.setColorIndex(1); // pixel on

  // Read the initial state of A (CLK)
  lastStateCLK = digitalRead(CLK2);
 
  // Clear the display:
  OurDisplay.clear();
  delay(1000);
  OurDisplay.setBrightness(7);
 
  // Call Interrupt Service Routine (ISR) updateEncoder() when any high/low change
  // is seen on A (CLK2) interrupt  (pin 2), or B (DT2) interrupt (pin 3)
 
  attachInterrupt(digitalPinToInterrupt(CLK2), updateEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(DT2), updateEncoder, CHANGE);
}
 
void startCountdownTimer() {
  if (counter <= 0) {
    Serial.println("Please set the countdown value to a positive number greater than 0.");
    tone(buzzer, toneFreq, 4500);  // ... and play the tone for one second
    delay(300);
    noTone(buzzer);
    return;
  }

  for(int countdown = counter; countdown > 0; countdown--){
    OurDisplay.showNumberDec(countdown);
    delay(100);

  }

  OurDisplay.setSegments(done);
  tone(buzzer, toneFreq, 4500);  // ... and play the tone for one second
  delay(3000);
  noTone(buzzer);
}

void displayCounter() {
  OurDisplay.showNumberDec(counter);
  delay(25); 
}
 
//  This is our ISR which has the job of responding to interrupt events
//
void updateEncoder(){
  // Read the current state of CLK
  currentStateCLK = digitalRead(CLK2);
 
  // If last and current state of CLK are different, then a pulse occurred;
  // React to only 0->1 state change to avoid double counting
  if (currentStateCLK != lastStateCLK  && currentStateCLK == 1){
 
    // If the DT state is different than the CLK state then
    // the encoder is rotating CW so INCREASE counter by 1
    if (digitalRead(DT2) == currentStateCLK) {
      counter ++;
      currentDir ="CW";
     
    } else {
      // Encoder is rotating CCW so DECREASE counter by 1
      counter --;
      currentDir ="CCW";
    }
 
    Serial.print("Direction: ");
    Serial.print(currentDir);
    Serial.print(" | Counter= ");
    Serial.println(counter);
  }
 
  // Remember last CLK state to use on next interrupt...
  lastStateCLK = currentStateCLK;
}

void checkSwitch() {
  int switchClick = digitalRead(SWITCH);
  if(switchClick == LOW) {
    // if 50ms have passed since last LOW pulse, it means that the
		// button has been pressed, released and pressed again
		if (millis() - lastButtonPress > 100) {
			Serial.println("Button pressed!");
      startCountdownTimer();
		}

		// Remember last button press event
		lastButtonPress = millis();
  }
}

int line1 = 10;
int line2 = 25;
int line3 = 40;
int line4 = 64;

void draw() {
  // graphic commands to redraw the complete screen should be placed here  
  My_u8g_Panel.setFont(u8g_font_unifont);
  My_u8g_Panel.drawStr( 0, line1, "Build");
  My_u8g_Panel.drawStr( 0, line2, "everything!");
  My_u8g_Panel.drawStr( 0, line3, "Invent safe!!");
  My_u8g_Panel.drawStr( 0, line4, "YOOOOOO!!!!");
}
 
void draw2() {
  // graphic commands to redraw the complete screen should be placed here  
  My_u8g_Panel.setFont(u8g_font_courB12);
  My_u8g_Panel.drawStr( 0, line1, "Build");
  My_u8g_Panel.drawStr( 0, line2, "everything!");
  My_u8g_Panel.drawStr( 0, line3, "Invent safe!!");
  My_u8g_Panel.drawStr( 0, line4, "YOOOOOO!!!!");
}
 
void displayOnOled() {
	if (millis() - lastScreenRefresh > 1000) {
    // picture loop
    if (showScreenA) {
      My_u8g_Panel.firstPage();
      do {
        draw();
      } while( My_u8g_Panel.nextPage() );
    } else {
      My_u8g_Panel.firstPage();
      do {
        draw2();
      } while( My_u8g_Panel.nextPage() );
    }
    showScreenA = !showScreenA;

    // Remember last screen refresh
		lastScreenRefresh = millis();
  }
}

void loop() {
  displayOnOled();
  displayCounter();
  checkSwitch();
}
