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

// pin 10 drives the BUZZER
#define BUZZER 10

// Define the OLED screens to use
#define SCREEN_STARTUP 0
#define SCREEN_COUNTDOWN 1
#define SCREEN_LIFTOFF 2
#define SCREEN_LIFTOFF_FOR_REAL 3

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
  tone(BUZZER, toneFreq, 4500);  // ... and play the tone for one second
  delay(3000);
  noTone(BUZZER);
  
  if (pause2 == 0) {
    delay(10000);
    resetDisplay();
  } else { 
    while(1);
  }
}


void playBeepBoop() {
  tone(BUZZER, 476, 1500);
  delay(1500);
  tone(BUZZER, 600, 1500);
  delay(1500);
  tone(BUZZER, 800, 1500);
  delay(1500);
  noTone(BUZZER);
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
