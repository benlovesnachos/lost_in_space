#include <Key.h>
#include <Keypad.h>
 
// Setup Keypad
const byte ROWS = 4;
const byte COLS = 4;
byte rowPins[ROWS] = {5, 4, 3, 2};
byte colPins[COLS] = {6, 7, 8, 9};
char buttons[ROWS][COLS] = {
  {'1', '2', '3', 'A'}, // Row 1
  {'4', '5', '6', 'B'}, // Row 2
  {'7', '8', '9', 'C'}, // Row 3
  {'*', '2', '#', 'D'} // Row 4
};

Keypad keyPad = Keypad(makeKeymap(buttons), rowPins, colPins, ROWS, COLS);

int tones[ROWS][COLS] = {   // a frequency tone for each button
    {31, 93, 147, 208},
    {247, 311, 370, 440},
    {523, 587, 698, 880},
    {1397, 2637, 3729, 4978}
};
 
int buzzer = 10;  // pin 10 drives the buzzer

void setup() {
  Serial.begin(9600);
}

void loop() {
  int toneFreq = 0;
  char result = keyPad.getKey();

  if (result) {
    for (byte j=0; j<ROWS; j++) {
      for (byte i=0; i<COLS; i++) {
          if (result == buttons[j][i]) {   // found it, get the corresponding tone
              toneFreq=tones[j][i];
              }
          
      }  // end i loop
    }  // end j loop

    Serial.print("Key: ");     //   send the result to serial...
    Serial.print(result);
    Serial.print("   Freq: ");
    Serial.println(toneFreq);  

    tone(buzzer, toneFreq, 500);  // ... and play the tone for a half second
    delay(500);
    noTone(buzzer);
  }

}

