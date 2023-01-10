// *****************************************************************************************
// Day 23 Launch System
//
// Prepare the countdown launch system.
//
// Integrated the countdown timer function with the rotary encoder.  The rotary encoder is
// used to set the time that will be used for the countdown.
// *****************************************************************************************
#include <TM1637Display.h>
#include <U8glib.h>


// MACRO DEFINES
#define numberOfMinutes(_time_) ((_time_ / 1000) / 60)
#define numberOfSeconds(_time_) ((_time_ / 1000) % 60)  

// Rotary Encoder Inputs
#define CLKrotary 2
#define DTrotary 3
#define SWrotary 4
 
// Define the OLED display connection pins
#define CLKsev 6
#define DIOsev 5

// Defind the DIP switches
#define PropulsionSW 9 //dip switch #1
#define AvionicsSW 8   //dip switch #2
#define ProceedSW 7    //dip switch #3

// pin 10 drives the AudibleConfirm
#define AudibleConfirm 10

// Define the OLED screens to use
#define SCREEN_STARTUP 0
#define SCREEN_COUNTDOWN 1
#define SCREEN_LIFTOFF 2
#define SCREEN_LIFTOFF_FOR_REAL 3
#define SCREEN_SHUTTLE 4

// Create display object of type TM1637Display:
TM1637Display My_display = TM1637Display(CLKsev, DIOsev);
// Define the OLED display
U8GLIB_SH1106_128X64  My_u8g_Panel(U8G_I2C_OPT_NONE); // I2C / TWI
 
uint8_t draw_state = 0;

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

// GLOBAL VARIABLES
unsigned long timeLimit = 10000;
unsigned long lastPeriodStart;
int counter = 0;    // keep a running tally of steps...
int currentStateCLK;
int lastStateCLK;
String currentDir ="";
unsigned long lastButtonPress = 0;
bool doCountdown = false;
int toneFreq = 23;

int pause = 0;
int pause2 = 0;

void setup() {
  // Set encoder pins as inputs
  pinMode(CLKrotary, INPUT);
  pinMode(DTrotary, INPUT);
  pinMode(SWrotary, INPUT_PULLUP);

  pinMode(PropulsionSW, INPUT);
  pinMode(AvionicsSW, INPUT);
  pinMode(ProceedSW, INPUT);

  // Setup Serial Monitor
  Serial.begin(9600);

  // flip screen, if required
  // My_u8g_Panel.setRot180();
  // assign default color value
  My_u8g_Panel.setColorIndex(1); // pixel on
  u8g_prepare();
  // My_u8g_Panel.firstPage();
  // do {
  //   u8g_starting_screen();
  // } while(My_u8g_Panel.nextPage());
  
  // Read the initial state of A (CLK)
  lastStateCLK = digitalRead(CLKrotary);
 
  // Clear the display:
  My_display.setBrightness(7);

  resetDisplay();
  // Call Interrupt Service Routine (ISR) updateEncoder() when any high/low change
  // is seen on A (CLKrotary) interrupt  (pin 2), or B (DTrotary) interrupt (pin 3)
  attachInterrupt(digitalPinToInterrupt(CLKrotary), updateEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(DTrotary), updateEncoder, CHANGE);
}

void startCountdownTimer() {
  if(!doCountdown) {
    // Calculate the time remaining
    drawScreen(SCREEN_COUNTDOWN);
    long countdownTime = counter * 1000;
    timeLimit = countdownTime + millis();
    doCountdown = true;
  }
}

void countdown() {
  if (!doCountdown) {
    // Set the time for countdown
    if(counter >= 0) {
      displayCountdown(counter * 1000);
    }
    return;
  }

  // Calculate the time remaining
  unsigned long timeRemaining = timeLimit - millis();
  Serial.print("Countdown started..: ");
  displayCountdown(timeRemaining);

  if(timeRemaining < 50){
    doCountdown = false;
    doLaunch();    
  }
}

void doLaunch() {
  if (pause2 == 0) {
    drawScreen(SCREEN_LIFTOFF);
  } else { 
    drawScreen(SCREEN_LIFTOFF_FOR_REAL);
  }
  
  if (pause2 == 0) {
    tone(AudibleConfirm, toneFreq, 4500);  // ... and play the tone for one second
    delay(1500);
    noTone(AudibleConfirm);
    delay(10000);
    resetDisplay();
  } else {
    delay(3000);
    //playVictoryFanfare();
    liftoff();
    while(1);
  }
}


void playBeepBoop() {
  tone(AudibleConfirm, 476, 1500);
  delay(1500);
  tone(AudibleConfirm, 600, 1500);
  delay(1500);
  tone(AudibleConfirm, 800, 1500);
  delay(1500);
  noTone(AudibleConfirm);
}

void playVictoryFanfare() {
  tone(AudibleConfirm, 261, 350); //C4
  delay(350);
  tone(AudibleConfirm, 261, 350); //C4
  delay(350);
  tone(AudibleConfirm, 261, 350); //C4
  delay(350);
  tone(AudibleConfirm, 261, 500); //C4
  delay(500);
  tone(AudibleConfirm, 207, 400); //A3 Flat
  delay(350);
  tone(AudibleConfirm, 233, 400); //B3 Flat
  delay(350);
  tone(AudibleConfirm, 261, 400); //C4
  delay(350);
  tone(AudibleConfirm, 233, 400); //B3 Flat
  delay(350);
  tone(AudibleConfirm, 261, 1500); //C4
  delay(1500);
  noTone(AudibleConfirm);
}


void liftoff() {
  // original was 69 beats per minute; one quarter note = 1.15 seconds = 1150 milliseconds
  // LIST OF LENGTHS OF NOTES
  int q = 1150; // quarter note
  int h = q*2; // half note
  int w = h*2; // whole note
  int e = q/2; // eighth note
  int s = e/2; // sixteenth note
  int t = q/3; // triplet eighth note
  // LIST OF FREQUENCY VALUES
  int c2 = 65;
  int g2 = 98;
  int c3 = 131;
  int e3 = 165;
  int g3 = 196;
  int c4 = 263;
  int e4 = 330;
  int g4 = 392;
  int a4 = 440;
  int b4 = 494;
  int c5 = 523;
  int d5 = 587;
  int e5 = 659;
  int eb5 = 622;
  int f5 = 699;
  int g5 = 784;
  int a5 = 880;
  int b5 = 989;
  int c6 = 1047;

  // START
  tone(AudibleConfirm, c4, h);
  delay(h);
  tone(AudibleConfirm, g4, h);
  delay(h);
  //
  tone(AudibleConfirm, c5, h+q+e);
  delay(h+q+e);
  delay(e); // eigth rest
  tone(AudibleConfirm, e5, s);
  delay(s);
  //
  tone(AudibleConfirm, eb5, w);
  delay(w);
  //
  // DRUMS
  tone(AudibleConfirm, c3, t);
  delay(t);
  tone(AudibleConfirm, g2, t);
  delay(t);
  tone(AudibleConfirm, c3, t);
  delay(t);
  tone(AudibleConfirm, g2, t);
  delay(t);
  tone(AudibleConfirm, c3, t);
  delay(t);
  tone(AudibleConfirm, g2, t);
  delay(t);
  tone(AudibleConfirm, c3, t);
  delay(t);
  tone(AudibleConfirm, g2, t);
  delay(t);
  tone(AudibleConfirm, c3, t);
  delay(t);
  tone(AudibleConfirm, g2, t);
  delay(t);
  tone(AudibleConfirm, c3, t);
  delay(t);
  tone(AudibleConfirm, g2, t);
  delay(t);
  //
  tone(AudibleConfirm, c4, h);
  delay(h);
  tone(AudibleConfirm, g4, h);
  delay(h);
  //
  tone(AudibleConfirm, c5, h+q+e);
  delay(h+q+e);
  delay(e); // eight rest
  tone(AudibleConfirm, eb5, s);
  delay(s);
  //
  tone(AudibleConfirm, e5, w);
  delay(w);
  //
  // DRUMS
  tone(AudibleConfirm, c3, t);
  delay(t);
  tone(AudibleConfirm, g2, t);
  delay(t);
  tone(AudibleConfirm, c3, t);
  delay(t);
  tone(AudibleConfirm, g2, t);
  delay(t);
  tone(AudibleConfirm, c3, t);
  delay(t);
  tone(AudibleConfirm, g2, t);
  delay(t);
  tone(AudibleConfirm, c3, t);
  delay(t);
  tone(AudibleConfirm, g2, t);
  delay(t);
  tone(AudibleConfirm, c3, t);
  delay(t);
  tone(AudibleConfirm, g2, t);
  delay(t);
  tone(AudibleConfirm, c3, t);
  delay(t);
  tone(AudibleConfirm, g2, t);
  delay(t);
  //
  tone(AudibleConfirm, c4, h);
  delay(h);
  tone(AudibleConfirm, g4, h);
  delay(h);
  //
  tone(AudibleConfirm, c5, h+q+e);
  delay(h+q+e);
  delay(e); // eigth rest
  tone(AudibleConfirm, e5, s);
  delay(s);
  //
  tone(AudibleConfirm, f5, w);
  delay(w);
  //
  tone(AudibleConfirm, a4, s);
  delay(s);
  tone(AudibleConfirm, b4, s);
  delay(s);
  tone(AudibleConfirm, c5, e+h);
  delay(e+h);
  tone(AudibleConfirm, d5, q);
  delay(q);
  //
  tone(AudibleConfirm, e5, e);
  delay(e);
  tone(AudibleConfirm, f5, e);
  delay(e);
  tone(AudibleConfirm, g5, e);
  delay(e);

  tone(AudibleConfirm, e4, e);
  delay(e);
  tone(AudibleConfirm, c4, e);
  delay(e);
  tone(AudibleConfirm, g3, e);
  delay(e);
  tone(AudibleConfirm, e3, e);
  delay(e);

  tone(AudibleConfirm, e5, s);
  delay(s);
  tone(AudibleConfirm, f5, s);
  delay(s);
  //
  tone(AudibleConfirm, g5, h);
  delay(h);
  tone(AudibleConfirm, a5, q+s); // decelerando very manually
  delay(q+s);
  delay(s); // sixteenth rest
  tone(AudibleConfirm, b5, q+e);
  delay(q+e);
  delay(e); // sixteenth rest
  //
  tone(AudibleConfirm, c6, w);
  delay(w);
  // END
  noTone(AudibleConfirm);
}
// *****************************************************************************************
//  This is our ISR which has the job of responding to interrupt events
void updateEncoder() {
  // Read the current state of CLK
  currentStateCLK = digitalRead(CLKrotary);
 
  // If last and current state of CLK are different, then a pulse occurred;
  // React to only 0->1 state change to avoid double counting
  if (currentStateCLK != lastStateCLK  && currentStateCLK == 1){
 
    // If the DT state is different than the CLK state then
    // the encoder is rotating CW so INCREASE counter by 1
    if (digitalRead(DTrotary) == currentStateCLK) {
      counter ++;
      currentDir ="CW";
     
    } else {
      // Encoder is rotating CCW so DECREASE counter by 1
      counter --;
      currentDir ="CCW";
    }

    if(counter < 0) {
      counter = 0;
    }
 
    Serial.print("Direction: ");
    Serial.print(currentDir);
    Serial.print(" | Counter= ");
    Serial.println(counter);
  }

  // if (counter > 0) {
  //   int timeToSet = counter * 1000;
  //   displayCountdown(timeToSet);
  // }

  if (counter == 20) {
    drawScreen(SCREEN_SHUTTLE);
  }

  // Remember last CLK state to use on next interrupt...
  lastStateCLK = currentStateCLK;
}

void checkSwitch() {
  int switchClick = digitalRead(SWrotary);
  if(switchClick == LOW) {
    // if 50ms have passed since last LOW pulse, it means that the
    // button has been pressed, released and pressed again
    if (millis() - lastButtonPress > 100) {
      Serial.println("Button pressed!");
      playBeepBoop();
      startCountdownTimer();
    }

    // Remember last button press event
    lastButtonPress = millis();
  }
}

// *****************************************************************************************
void resetDisplay() {
  My_display.clear();
  drawScreen(SCREEN_STARTUP);
}

void updateCountdown() {
  if(counter >=0 && !doCountdown) {
    long countdownTime = counter * 1000;
    displayCountdown(countdownTime);
  }
}

void displayCountdown(long timeRemaining) {
  if(doCountdown) { Serial.println(timeRemaining); }
  // To display the countdown in mm:ss format, separate the parts
  int seconds = numberOfSeconds(timeRemaining);
  int minutes = numberOfMinutes(timeRemaining);

  // This displays the seconds in the last two places
  My_display.showNumberDecEx(seconds, 0, true, 2, 2);
  // Display the minutes in the first two places, with colon
  //My_display.showNumberDecEx(minutes, 0x80>>3, true, 2, 0);
  My_display.showNumberDecEx(minutes, 0x40, true, 2, 0);
}

void drawScreen(int screen) {
  u8g_prepare();
  My_u8g_Panel.firstPage();  
  do {
    // u8g_liftoff_msg();
    switch(screen) {
      case SCREEN_STARTUP: u8g_blank_screen(); break;
      case SCREEN_COUNTDOWN: u8g_starting_screen(); break;
      case SCREEN_LIFTOFF: u8g_liftoff_msg(); break;
      case SCREEN_LIFTOFF_FOR_REAL: u8g_liftoff_for_real_msg(); break;
      case SCREEN_SHUTTLE: u8g_shuttle(); break;
    }
    
  } while( My_u8g_Panel.nextPage() );
}

void u8g_prepare(void) {
  My_u8g_Panel.setFont(u8g_font_6x10);
  My_u8g_Panel.setFontRefHeightExtendedText();
  My_u8g_Panel.setDefaultForegroundColor();
  My_u8g_Panel.setFontPosTop();
}

// OLED Display output
void u8g_blank_screen(void) {
  // graphic commands to redraw the complete screen should be placed here  
  My_u8g_Panel.setFont(u8g_font_unifont);
  My_u8g_Panel.drawStr( 0, 22, "Press button");
  My_u8g_Panel.drawStr( 10, 35, "to launch...");
}

void u8g_starting_screen(void) {
  // graphic commands to redraw the complete screen should be placed here  
  My_u8g_Panel.setFont(u8g_font_unifont);
  My_u8g_Panel.drawStr( 0, 22, "Counting down...");
}

void u8g_liftoff_msg(void) {
  // graphic commands to redraw the complete screen should be placed here  
  My_u8g_Panel.setFont(u8g_font_unifont);
  My_u8g_Panel.drawStr( 0, 22, "LIFTOFF!!");
  My_u8g_Panel.drawStr( 0, 44, "End of sim");
}

void u8g_liftoff_for_real_msg(void) {
  // graphic commands to redraw the complete screen should be placed here  
  My_u8g_Panel.setFont(u8g_font_unifont);
  My_u8g_Panel.drawStr( 0, 12, "LIFTOFF!!");
  My_u8g_Panel.drawStr( 0, 34, "(this is not ");
  My_u8g_Panel.drawStr( 0, 54, " a test)");
}

void u8g_shuttle(void) {
  // u8g_prepare();
  My_u8g_Panel.setFont(u8g_font_unifont);
  My_u8g_Panel.drawStr( 0, 0, "Space Shuttle");
  My_u8g_Panel.drawTriangle(24,58,40,61,56,58);
  My_u8g_Panel.drawBox(24,56,32,3);
  My_u8g_Panel.drawTriangle(24,56,32,56,32,45);
  My_u8g_Panel.drawBox(32,45,16,11);
  My_u8g_Panel.drawTriangle(48,56,56,56,48,45);
  My_u8g_Panel.drawTriangle(32,45,40,45,40,22);
  My_u8g_Panel.drawTriangle(40,45,48,45,40,22);
  My_u8g_Panel.drawBox(35,61,4,3);
  My_u8g_Panel.drawBox(41,61,4,3);
}
// *****************************************************************************************
void readDip() {
  if(digitalRead(PropulsionSW) == 1 &&
     digitalRead(AvionicsSW) == 1 &&
     digitalRead(ProceedSW) == 1) {
      pause = 1;
  }
  if (pause == 1 && pause2 == 0) {
      playBeepBoop();   // after a set of warning tones and brief pause
      pause2 = 1;       // the countdown will commence
  } else if (pause2 == 1) {
    startCountdownTimer();      // HANG ON!!!
  }  
}

// *****************************************************************************************
void loop() {
  countdown();
  // displayOnOled();
  // displayCountdown();
  checkSwitch();
  readDip();
}
