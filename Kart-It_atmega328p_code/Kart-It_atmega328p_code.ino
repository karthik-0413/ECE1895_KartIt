////////////////////////////////////////////////////////////////////////////////////////
//
//  University of Pittsburgh
//  Swanson School of Engineering
//  ECE 1895: Junior Design
//  Prof. Samuel Dickerson
//  Project 2: Bop-It
//
//  Group: Kart-It (Delta)
//  Members: William Roper, Karthik Raja, Zachary McPherson
//
//  This is the arduino code for the atmega328p used in our design
//  This program receives a command (via serial) from a Raspberry Pi
//  After receiving a command, the atmega polls the following inputs:
//    - Rotary Encoder (1)
//    - Potentiometer (2)
//    - Limit Switch (2)
//
//  If the player gives an incorrect input, a FAIL signal is sent to the Raspberry Pi via GPIO
//  If the player gives the correct input, a PASS signal is sent to the Raspberry Pi via GPIO
//
//  The atmega is hooked up to a DF-player  
//  Once a command is received, a track announces the command via the DF-player
//
////////////////////////////////////////////////////////////////////////////////////////

// <#include <SoftwareSerial.h>
// Include the library
#include <TM1637Display.h>
// #include "mp3tf16p.h"
#include <NeoSWSerial.h>
#include <avr/interrupt.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>


// Display:
// Seven Segment -> Score
// LCD -> Commands

// Initialize Sensor Pins Here
// const int PiRXPin = 0;                   // Raspberry Pi RX Pin
// const int PiTXPin = 1;                   // Audio Output Pin
const int ASignal = 2;                  // Rotary Encoder for Steering
const int BSignal = 3;                    // Rotary Encoder for Steering
const int RightArrowSelect = 4;          // System Requirements
const int LeftArrowSelect = 5;      // Joystick Button
const int limitSwitchTopPin = 6;     // Hall Effect Sensor
const int limitSwitchBottomPin = 7;  // Hall Effect Sensor6
const int TMLCLK = 8;                  // Seven Segment Display Clock
const int TMDATA = 9;                  // Seven Segment Display Data
const int RXPin = 10;
const int TXPin = 11;
const int PiPassPin = 12;
const int PiFailPin = 13;
const int PotentiometerGasPin = A0;        // Limit Switch
const int PotentiometerBrakePin = A1;      // Limit Switch
const int startGameButton = A2;               // Joystick X-Axis
// const int BackUpPiRXPin = A3;              // Audio Output Pin
// const int SDAPin = A4;                        // I2C
// const int SCLPin = A5;                        // I2C


// Setting up DFPlayer Mini
// SoftwareSerial softwareSerial(RXPin, TXPin);  // Setting up DFPlayer Mini
// MP3Player mp3(RXPin, TXPin);

// Define the LCD address (default is 0x27 or 0x3F, depending on your module)
LiquidCrystal_I2C lcd(0x27, 16, 2); // 16x2 LCD

// Seven Segment Display
TM1637Display display = TM1637Display(TMLCLK, TMDATA);

NeoSWSerial mySerial(RXPin, TXPin); // RX = 6, TX = 7


// Variables for Logic
bool systemOn = false;              // System On Flag
bool startgame = false;              // Game Start Flag
bool lostGame = false;              // If the user is unsuccessful in responding to a command either by not responding in time or by providing an incorrect response
bool correctCommand = true;         // Flag to keep track if the user did the right command
bool speedBoost = false;      // Flag to keep track of powerup being activated
bool pointsDoubleActive = false;    // Flag to keep track of powerup being activated
bool previousStartButtonStatus = false;
bool previousRightState = LOW;
bool previousLeftState = LOW;
bool startButtonPreviouslyPressed = false; // Track previous state
bool freestyle = false;

int totalPoints = 0;                // Points Tracker
int currentTask;                // Random Int Function picks number between 1-3 for Command
int speedCounter = 0;         // 5 seconds of no timer for player
int pointsDoubleCounter = 5;       // 5 seconds of double points for player
int counter = 0;                    // Counter variable for the rotary encoder
int finalCounter = 0;
int lastA = LOW;                    // Keep track of previous ASignal value for the rotary encoder
int previousCommand = 0;            // Keep track of previous command to avoid back-to-back repeats
int currentTrack = 10;               // Keep track of current track selected
int arrowCounter = 0;               // Keep track of relative arrow presses for track select
int commandCounter = 0;           // Index for Current Command
int startButtonPressCount = 0;    // Count the number of button presses
int volume = 7;

float timeBetweenTasks;       // Time interval between commands becomes smaller with each successful attempt (seconds)

String currentDir = "";             // Variable for direction of steering wheel
String gameTracks[4] = {"Freestyle", "Rainbow Road", "Bowser's Castle", "Baby Park"};

// Hard coded tracks for Mariokart
// const int RainbowRoad[82] = {
//     4, 2, 4, 2, 4, 2, 6, 3, 5, 6, 3, 5, 5, 4, 2, 4, 2, 6, 3, 1,
//     4, 2, 6, 3, 5, 4, 2, 6, 3, 1, 6, 3, 1, 6, 3, 1, 4, 2, 4, 2,
//     6, 3, 5, 4, 2, 6, 3, 1, 6, 3, 1, 4, 2, 6, 3, 5, 4, 2, 4, 2,
//     6, 3, 5, 6, 3, 5, 1, 1, 6, 3, 1, 4, 2, 4, 2, 6, 3, 1, 4, 2,
//     4, 2
// };

// const int BowserCastle[77] = {
//     4, 2, 4, 2, 6, 3, 5, 4, 2, 4, 2, 6, 3, 1, 4, 2, 6, 3, 5, 4,
//     2, 6, 3, 5, 4, 2, 6, 3, 1, 4, 2, 6, 3, 1, 4, 2, 6, 3, 5, 4,
//     2, 6, 3, 1, 4, 2, 6, 3, 1, 4, 2, 6, 3, 5, 4, 2, 4, 2, 6, 3,
//     1, 6, 3, 1, 4, 2, 4, 2, 4, 2, 6, 3, 5, 4, 2, 4, 2
// };

// const int BabyPark[26] = {
//     4, 2, 4, 2, 6, 3, 1, 6, 3, 1, 
//     4, 2, 4, 2, 4, 2, 6, 3, 1, 6, 
//     3, 1, 4, 2, 4, 2
// };

/////////////////////////////////
//     SplitWords Method      //
///////////////////////////////
String* splitWords(String input, int numWords = 0) {
  // Remove any leading/trailing whitespace.
  input.trim();

  // Count the number of words.
  numWords = 0;
  int length = input.length();
  int index = 0;
  while (index < length) {
    // Skip any spaces.
    while (index < length && input.charAt(index) == ' ') {
      index++;
    }
    // If we have reached a non-space character, it's the start of a word.
    if (index < length) {
      numWords++;
      // Skip the characters of the word.
      while (index < length && input.charAt(index) != ' ') {
        index++;
      }
    }
  }
  
  // Allocate an array for the words.
  String* words = new String[numWords];

  // Extract and store each word.
  int wordIndex = 0;
  index = 0;
  while (index < length && wordIndex < numWords) {
    // Skip leading spaces.
    while (index < length && input.charAt(index) == ' ') {
      index++;
    }
    int start = index;
    // Move index to the end of the word.
    while (index < length && input.charAt(index) != ' ') {
      index++;
    }
    if (start < index) {
      words[wordIndex++] = input.substring(start, index);
    }
  }
  
  return words;
}

void playTrack(int trackNumber) {
  byte command[] = {0x7E, 0xFF, 0x06, 0x03, 0x00, (byte)(trackNumber >> 8), (byte)(trackNumber), 0xEF};
  mySerial.write(command, sizeof(command));
  //Serial.print("Playing track ");
  //Serial.println(trackNumber);
}

bool isPlaying() {
  byte command[] = {0x7E, 0xFF, 0x06, 0x42, 0x00, 0x00, 0x00, 0xEF}; // Query playback status command
  mySerial.write(command, sizeof(command));

  delay(100); // Wait for response

  if (mySerial.available()) {
    byte response = mySerial.read();
    return response == 0x01; // If response byte indicates playback is active
  }
  return false; // Default to "not playing" if no response
}

int checkUserResponse(int task) {
    switch (task) {
      // 1 = Turn Right
      case 5:
        // Serial.println("In 1");

        if (analogRead(PotentiometerGasPin) < 400) {
          //Serial.println("Gas Instead");
          return 1;
        } else if (analogRead(PotentiometerBrakePin) < 400) {
          //Serial.println("Brake Instead");
          return 1;
        } else if (digitalRead(limitSwitchTopPin) == LOW) {
          //Serial.println("Shift Up Instead");
          return 1;
        } else if (digitalRead(limitSwitchBottomPin) == LOW) {
          //Serial.println("Shift Down Instead");
          return 1;
        } else if (currentDir == "Clockwise") {
          //Serial.println("Turn Right Instead");
          return 1;
        } else if (currentDir == "Counterclockwise") {
          return 2;
        } else {
          return 3;
        }
        break;
        
      // 2 = Gas It
      case 2:

        // Serial.println("In 2");

        if (currentDir != "") {
          //Serial.println("Turn Instead");
          return 1;
        } else if (analogRead(PotentiometerBrakePin) < 400) {
          //Serial.println("Brake Instead");
          return 1;
        } else if (digitalRead(limitSwitchTopPin) == LOW) {
          //Serial.println("Shift Up Instead");
          return 1;
        } else if (digitalRead(limitSwitchBottomPin) == LOW) {
          //Serial.println("Shift Down Instead");
          return 1;
        } else if (currentDir != "") {
          //Serial.println("Turn Instead");
          return 1;
        } else if (analogRead(PotentiometerGasPin) < 400) {
          return 2;
        } else {
          return 3;
        }
        break;

      // 3 = Brake It
      case 3: 
        // Serial.println("In 3");

        if (currentDir == "Counterclockwise") {
          //Serial.println("Turn Left Instead");
          return 1;
        } else if (analogRead(PotentiometerGasPin) < 400) {
          //Serial.println("Gas Instead");
          return 1;
        } else if (digitalRead(limitSwitchTopPin) == LOW) {
          //Serial.println("Shift Up Instead");
          return 1;
        } else if (digitalRead(limitSwitchBottomPin) == LOW) {
          //Serial.println("Shift Down Instead");
          return 1;
        } else if (currentDir == "Clockwise") {
          //Serial.println("Turn Right Instead");
          return 1;
        } else if (analogRead(PotentiometerBrakePin) < 400) {
          return 2;
        } else {
          return 3;
        }
        break;
      
      // 4 = Shift Up
      case 4:

        // Serial.println("In 4");

        if (currentDir == "Counterclockwise") {
          //Serial.println("Turn Left Instead");
          return 1;
        } else if (analogRead(PotentiometerGasPin) < 400) {
          //Serial.println("Gas Instead");
          return 1;
        } else if (analogRead(PotentiometerBrakePin) < 400) {
          //Serial.println("Brake Instead");
          return 1;
        } else if (digitalRead(limitSwitchBottomPin) == LOW) {
          //Serial.println("Shift Down Instead");
          return 1;
        } else if (currentDir == "Clockwise") {
          //Serial.println("Turn Right Instead");
          return 1;
        } else if (digitalRead(limitSwitchTopPin) == LOW) {
          return 2;
        } else {
          return 3;
        }
        break;
        
      // 5 = Turn left
      case 1:

        // Serial.println("In 5");

        if (analogRead(PotentiometerGasPin) < 400) {
          //Serial.println("Gas Instead");
          return 1;
        } else if (analogRead(PotentiometerBrakePin) < 400) {
          //Serial.println("Brake Instead");
          return 1;
        } else if (digitalRead(limitSwitchTopPin) == LOW) {
          //Serial.println("Shift Up Instead");
          return 1;
        } else if (digitalRead(limitSwitchBottomPin) == LOW) {
          //Serial.println("Shift Down Instead");
          return 1;
        } else if (currentDir == "Counterclockwise") {
          //Serial.println("Left Instead");
          return 1;
        } else if (currentDir == "Clockwise") {
          return 2;
        } else {
          return 3;
        }
        break;

      // 6 = Shift Down
      case 6:

        // Serial.println("In 6");

        if (currentDir == "Counterclockwise") {
          return 1;
        } else if (analogRead(PotentiometerGasPin) < 400) {
          //Serial.println("Gas Instead");
          return 1;
        } else if (analogRead(PotentiometerBrakePin) < 400) {
          //Serial.println("Brake Instead");
          return 1;
        } else if (digitalRead(limitSwitchTopPin) == LOW) {
          //Serial.println("Shift Up Instead");
          return 1;
        } else if (currentDir == "Clockwise") {
          //Serial.println("Turn Right Instead");
          return 1;
        } else if (digitalRead(limitSwitchBottomPin) == LOW) {
          return 2;
        } else {
          return 3;
        }
        break;

      default: 
        return 3;
        break;
  }
} 

void handle_A() {
  int currentA = digitalRead(ASignal);
  int currentB = digitalRead(BSignal);

  if (currentA != lastA) {
    if (currentA == HIGH) {
        if (currentB == LOW) {
            counter++;
            //currentDir = "Clockwise";
        } else {
            counter--;
            //currentDir = "Counterclockwise";
        }
    } else {
        if (currentB == LOW) {
            counter--;
            //currentDir = "Counterclockwise";
        } else {
            counter++;
            //currentDir = "Clockwise";
        }
    }
    lastA = currentA;

    if (counter == 300 || counter == -300) {
      //Serial.print("Encoder Counter: ");
      //Serial.println(counter);  
    }


    // Positive Counter -> Clockwise = FLIPPED IN ORDER TO WORK ON PHYSICAL DESIGN
    if (counter > 300) {
      currentDir = "Clockwise";
    } else if (counter < -300) {
      currentDir = "Counterclockwise";
    } else {
      currentDir = "";
    }
  }
}

/////////////////////////////////
//        Setup Method        //
///////////////////////////////
void setup() {

  // Pi & DF Player Serials
  Serial.begin(9600);
  mySerial.begin(9600);     // Communication with DFPlayer Mini
  
  // Wait for Serial to initialize
  delay(1000);

  // Play MP3 file
  playTrack(11); // Replace '1' with the desired track number

  // Define Pin Modes of All Sensors
  pinMode(PotentiometerGasPin, INPUT);
  pinMode(PotentiometerBrakePin, INPUT);
  pinMode(limitSwitchTopPin, INPUT_PULLUP);
  pinMode(limitSwitchBottomPin, INPUT_PULLUP);
  pinMode(ASignal,INPUT_PULLUP);
	pinMode(BSignal,INPUT_PULLUP);

  // Pin Modes for Pi Pins
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);

  // Define Pin Modes for Buttons for Track Select
  //pinMode(RightArrowSelect, INPUT);
  //pinMode(LeftArrowSelect, INPUT);

  // Define Pin Modes for Other Buttons
  //pinMode(startGameButton, INPUT);
  
  // mp3.initialize();

  lcd.init();       // Initialize the LCD
  lcd.backlight();  // Turn on the backlight
  lcd.setCursor(0, 0);  // Set cursor to first row, first column

  // Set the brightness to 5 (0=dimmest 7=brightest)
	display.setBrightness(5);
	display.clear();
  display.showNumberDec(0);


	// Read the initial state of A Signal (for Rotary Encoder)
  lastA = digitalRead(ASignal);

  attachInterrupt(digitalPinToInterrupt(ASignal), handle_A, CHANGE);
  attachInterrupt(digitalPinToInterrupt(BSignal), handle_A, CHANGE);

  //randomSeed(analogRead(A5));

  // mp3.playTrackNumber(11, volume);
  // delay(7000);
  
}

/////////////////////////////////
//          Main Loop         //
///////////////////////////////
void loop() {

  // Awaiting serial command from Pi
  if (Serial.available())
  {
    // Read hardware serial
    String piMsg = Serial.readStringUntil('\n');
    piMsg.trim();
      
///////////////////////////////
//       Map Selection      //
/////////////////////////////

    if (piMsg == "Donut Plains" || piMsg == "Mario Circuit" || piMsg == "Ghost Valley" || piMsg == "Bowser's Castle" || piMsg == "Rainbow Road" || piMsg == "Gamemode: Freestyle"){
      
      //Serial.println("in map change");

      // Set Freestyle Flags
      if (piMsg == "Gamemode: Freestyle") {
        freestyle = true;
        timeBetweenTasks = 6.5f;
      }
      else {
        freestyle = false;
        timeBetweenTasks = 8.0f;
      }

      // Reset totalPoints
      totalPoints = 0;
      display.showNumberDec(totalPoints);

      // Split words
      String* words = splitWords(piMsg);

      ///////////////////////////////
      //     Print to mini LCD    //
      /////////////////////////////
      lcd.clear();

      // Print first line
      lcd.setCursor(0, 0);
      lcd.print(words[0]);

      // Print second line
      lcd.setCursor(0, 1);
      lcd.print(words[1]);


///////////////////////////////
//     Task Processing      //
/////////////////////////////

/*
  5 = Turn Right
  2 = Gas It
  3 = Brake It
  4 = Shift Up
  1 = Turn Left
  6 = Shift Down
*/

    } else if (piMsg == "1" || piMsg == "2" || piMsg == "3" || piMsg == "4" || piMsg == "5" || piMsg == "6"){

      ///////////////////////////////
      //         Set Task         //
      /////////////////////////////
      currentTask = piMsg.toInt();
      //Serial.print("current task: ");
      //Serial.println(currentTask);
      playTrack(currentTask);
      // delay(1500);

      ///////////////////////////////
      //    Poll User Response    //
      /////////////////////////////
      bool taskCompleted = false;
      int goodResponse = 0;
      
      // Set timer
      long startTime = millis();
      
      // Continuously Check User Response
      //Serial.print("----polling ");
      //Serial.print(currentTask);
      //Serial.println("----");
      while (millis() - startTime < timeBetweenTasks * 1000) {
        // 1 = Bad Response
        // 2 = Correct Response
        // 3 = No Response Yet
        goodResponse = checkUserResponse(currentTask);
        if (goodResponse == 2) {
          taskCompleted = true;
          playTrack(7); // Replace '1' with the desired track number
          // delay(2000);
          break;
        } else if (goodResponse == 1) {
          //taskCompleted = false;
          playTrack(8); // Replace '1' with the desired track number
          // delay(2000);
          break;
        }
      }
      //Serial.println("----end of loop----");

      ///////////////////////////////
      //          PASSED          //
      /////////////////////////////

      if (taskCompleted) {

        //Serial.println("Task Completed");
        // Increment Point total if <99
        if (freestyle) {
          if (totalPoints < 99){
            totalPoints++;
          }
        } else {
          totalPoints++;
        }

        display.showNumberDec(totalPoints);
          
        // Change time user has to complete task
        if (freestyle) {
          if (timeBetweenTasks > 2.0 && !(totalPoints % 10))
          {
            timeBetweenTasks -= 0.5;
          }
        } else {
          if ((timeBetweenTasks - 0.5) >= 3.0)
              if (currentTask == 2) {
                timeBetweenTasks -= 0.5;
              } else if (currentTask == 3) {
                timeBetweenTasks += 0.25;
              }
        }
        
        // Write PASS to Pi
        //Serial.println("---Write Pass---");
        digitalWrite(12, HIGH);
        delay(100);
        digitalWrite(12, LOW);

      ///////////////////////////////
      //          FAILED          //
      /////////////////////////////
      
      } else if (!taskCompleted) {
        // playTrack(8); // Replace '1' with the desired track number
        // delay(2000);
        //Serial.println("Task Incomplete");

        ////Serial.println("---Write Fail---");
        // Write FAIL to Pi
        digitalWrite(13, HIGH);
        delay(100);
        digitalWrite(13, LOW);
        
      }

    //delay(100);  // Delay

    }
  }
}

//*********************************
//*********** FUNCTIONS ***********
//*********************************

// Function to check if the user performed the correct response


// Function to randomly generate power-ups
/*void applyPowerUp() {
    int powerUpChance = random(1, 11);

    switch (powerUpChance) {
        // Bonus five points for one time
        case 1:
            //Serial.println("Power-Up: 5 Bonus Points!");
            totalPoints += 5;
            display.showNumberDec(totalPoints);
            Serial.print("Points: ");
            Serial.println(totalPoints);
            lcd.setCursor(0, 1);  // Set cursor to first row, first column
            lcd.print("+5 Points!");
            delay(2000);
            break;
        // Minus One Points for one time
        case 2:
            Serial.println("Power-Up: -1 Point!");
            totalPoints -= 1;
            display.showNumberDec(totalPoints);
            Serial.print("Points: ");
            Serial.println(totalPoints);
            lcd.setCursor(0, 1);  // Set cursor to first row, first column
            lcd.print("-1 Point!");
            delay(2000);
            break;
        // Gas It for five commands
        case 3:
          if (!speedBoost) {
            Serial.println("Power-Up: Speed Boost!");
            speedBoost = true;
            lcd.setCursor(0, 1);  // Set cursor to first row, first column
            lcd.print("Speed Boost!");
            delay(2000);
            break;
          }
        default:
          break;
    }
}*/






