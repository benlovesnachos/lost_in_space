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

void setup() {
  Serial.begin(9600);
}

void loop() {
  readSingle();
  // readMultiple();
}

void readMultiple() {
  char result = keyPad.getKeys();
  if (result) {
    Serial.println(result);
  }
}


void readSingle() {
  char result = keyPad.getKey();
  if (result) {
    Serial.println(result);
  }
}

