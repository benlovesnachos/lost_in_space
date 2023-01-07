#include <Key.h>
#include <Keypad.h>

int MIN_LIGHT = 0;
int MAX_LIGHT = 125;

int buzzer = 8;                          //sound output pin
int redPin = 11;                         //  PWM color output pins
int greenPin = 10;
int bluePin = 9;
 
const byte ROWS = 4;
const byte COLS = 4;
const byte PassLength = 4;               // passwords will be 4 characters long
char currentPassword[PassLength] = {'0','0','0','0'}; // default before setting
 
char buttons[ROWS][COLS] = {
  {'1', '2', '3', 'A'},  // 1st row
  {'4', '5', '6', 'B'},  // 2nd row
  {'7', '8', '9', 'C'},  // 3rd row
  {'*', '0', '#', 'D'}   // 4th row
};
 
byte rowPins[ROWS] = {5, 4, 3, 2};
byte colPins[COLS] = {6, 7, 12, 13};  // reassigned two of the column pins
 
Keypad securityPad = Keypad(makeKeymap(buttons), rowPins, colPins, ROWS, COLS);

//  ************************************************************************
//  process a login attempt ************************************************
int unlockMode() {
  char result;
  char currentEntry[PassLength];

  RGB_color(0, 0, 0);
  Serial.println("Type password to continue");
  delay(500);
  
  for(int i = 0; i < PassLength; i++) {
    while(!(result = readKeyPress())) {} // wait indefinitely for keypad input of any kind
    currentEntry[i] = result;
    Serial.print("*");                   // print a phantom character for a successful keystroke
  }                                      //  done after 4 characters are accepted
  
  Serial.println("");
  for(int i = 0; i < PassLength; i++) {  // check the Password
    if(currentEntry[i] != currentPassword[i]) {
      return -1;                         // a digit didn't match – FAIL
    }
  }
  return 0;                              //  0 means succeeded
}

void changePassword() {
  char result;
  Serial.println("Please enter a new password: ");

  for(int i = 0; i < PassLength; i++) {
    solidBlue();
    while(!(result = readKeyPress())) {} // wait indefinitely for keypad input of any kind
    currentPassword[i] = result;
    Serial.print("*");                   // print a phantom character for a successful keystroke
  }                                      //  done after 4 characters are accepted

  Serial.println("");
  Serial.println("Password successfully changed!");
  playSuccess();  
  flashGreen();
}

void failChangePassword() {
  Serial.println("Password change failed.");
  playError();  
  flashRed();
}

char readKeyPress() {
  char result = securityPad.getKey();
  if (result != NO_KEY) {
    playInput();
    flashBlue();
  }
  return result;
}

void accessDenied() {
  Serial.println("Access Denied.");
  printoutInstructions();
  printoutKeys();
  RGB_color(125, 0, 0); // LED to RED
  playError();
  RGB_color(0, 0, 0);   // LED to off
}

void accessGranted() {
  Serial.println("Welcome, authorized user. You may now begin using the system.");
  RGB_color(0, 125, 0); // LED to GREEN
  playSuccess();  
  RGB_color(0, 0, 0);   // LED to off
}
 
//  ************************************************************************
//  audio feedback *********************************************************
void playSuccess() {
  tone(buzzer, 1000, 200);
  delay(200);
  tone(buzzer, 2700, 1000);
  delay(1000);
  noTone(buzzer);
}
 
void playError() {
  tone(buzzer, 147, 200);
  delay(200);
  noTone(buzzer);
}
 
void playInput() {
  tone(buzzer, 880, 200);
  delay(50);
  noTone(buzzer);
}
 
//  ************************************************************************
//  LED functions **********************************************************
void RGB_color(int red_value, int green_value, int blue_value) {
  // Validate color values to keep within bounds.
  red_value = validateLightValue(red_value);
  green_value = validateLightValue(green_value);
  blue_value = validateLightValue(blue_value);
  
  analogWrite(redPin, red_value);
  analogWrite(greenPin, green_value);
  analogWrite(bluePin, blue_value);
}

int validateLightValue(int check_value) {
  if (check_value < MIN_LIGHT) {
    return MIN_LIGHT;
  } else if (check_value > MAX_LIGHT) {
    return MAX_LIGHT;
  } else {
    return check_value;
  }
}

void flashBlue() {
  RGB_color(0, 0, 125);
  delay(100);
  RGB_color(0, 0, 0);
}

void solidBlue() {
  RGB_color(0, 0, 125);
}

void flashGreen() {
  RGB_color(0, 125, 0);
  delay(250);
  RGB_color(0, 0, 0);
}

void flashRed() {
  RGB_color(125, 0, 0);
  delay(250);
  RGB_color(0, 0, 0);
}

//  ************************************************************************
//  User interactions ******************************************************
void displayBootup() {
  Serial.print("Booting up");
  for (int i = 0; i < 3; i++) {
    Serial.print(".");
    delay(750);
  }
  Serial.println(".");
  Serial.println("Welcome to rekOS");
  Serial.println("Best of luck in saving yourself!");
  Serial.println("");
  delay(1750);
}

void printoutInstructions() {
  Serial.println("Press * to set a new password.");
  Serial.println("Press # to access the system with the existing one.");
}

void printoutKeys() {
  Serial.println("+- Keypad-+");
  Serial.println("| 1 2 3 A |");
  Serial.println("| 4 5 6 B |");
  Serial.println("| 7 8 9 C |");
  Serial.println("| * 0 # D |");
  Serial.println("+---------+");
  Serial.println("");
  Serial.println("");
}

//  ************************************************************************
//  ************************************************************************
void setup() {
  pinMode(redPin, OUTPUT);               // designate pins for PWM LED output
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
 
  Serial.begin(9600);                    // Begin monitoring via the serial monitor

  RGB_color(125, 125, 125);              //set LED to white on startup...
  displayBootup();
  RGB_color(0, 0, 0);                    //... and off again
  delay(500);

  printoutInstructions();
  printoutKeys();
}

void loop() {
  char result = readKeyPress();
  if (result != NO_KEY) {
    switch (result) {
      case '*':
        if (unlockMode() < 0) {
          failChangePassword();
        } else {
          changePassword();
        }
        break;
      case '#':
        if(unlockMode() < 0) {
          accessDenied();
        } else {
          accessGranted();
        }
        break;
      default:
        printoutInstructions();
        printoutKeys();      
        break;
    }
  }
}
