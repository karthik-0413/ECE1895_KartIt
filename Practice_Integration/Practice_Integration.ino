// <#include <SoftwareSerial.h>
// Include the library
#include <TM1637Display.h>
#include "mp3tf16p.h"
#include <avr/interrupt.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Display:
// Seven Segment -> Score
// LCD -> Commands

// Initialize Sensor Pins Here
const int PiRXPin = 0;                   // Raspberry Pi RX Pin
const int PiTXPin = 1;                   // Audio Output Pin
const int ASignal = 2;                  // Rotary Encoder for Steering
const int BSignal = 3;                    // Rotary Encoder for Steering
const int RightArrowSelect = 4;          // System Requirements
const int LeftArrowSelect = 5;      // Joystick Button
const int limitSwitchTopPin = 6;     // Hall Effect Sensor
const int limitSwitchBottomPin = 7;  // Hall Effect Sensor
const int TMLCLK = 8;                  // Seven Segment Display Clock
const int TMDATA = 9;                  // Seven Segment Display Data
const int RXPin = 10;
const int TXPin = 11;
const int PiPassPin = 12;
const int PiFailPin = 13;
const int PotentiometerGasPin = A0;        // Limit Switch
const int PotentiometerBrakePin = A1;      // Limit Switch
const int startGameButton = A2;               // Joystick X-Axis
const int BackUpPiRXPin = A3;              // Audio Output Pin
const int SDAPin = A4;                        // I2C
const int SCLPin = A5;                        // I2C

// Create an array that turns all segments ON
const uint8_t allON[] = {0xff, 0xff, 0xff, 0xff};

// Create an array that turns all segments OFF
const uint8_t allOFF[] = {0x00, 0x00, 0x00, 0x00};


// Setting up DFPlayer Mini
// SoftwareSerial softwareSerial(RXPin, TXPin);  // Setting up DFPlayer Mini
MP3Player mp3(RXPin, TXPin);

// Define the LCD address (default is 0x27 or 0x3F, depending on your module)
LiquidCrystal_I2C lcd(0x27, 16, 2); // 16x2 LCD

// Seven Segment Display
TM1637Display display = TM1637Display(TMLCLK, TMDATA);


// Variables for Logic
bool systemOn = false;              // System On Flag
bool startgame = false;              // Game Start Flag
bool lostGame = false;              // If the user is unsuccessful in responding to a command either by not responding in time or by providing an incorrect response
bool correctCommand = true;         // Flag to keep track if the user did the right command
bool timeFreezeActive = false;      // Flag to keep track of powerup being activated
bool pointsDoubleActive = false;    // Flag to keep track of powerup being activated
bool previousStartButtonStatus = false;
bool previousRightState = LOW;
bool previousLeftState = LOW;

int totalPoints = 0;                // Points Tracker
int currentTask = 1;                // Random Int Function picks number between 1-3 for Command
int timeFreezeCounter = 5;         // 5 seconds of no timer for player
int pointsDoubleCounter = 5;       // 5 seconds of double points for player
int counter = 0;                    // Counter variable for the rotary encoder
int finalCounter = 0;
int lastA = LOW;                    // Keep track of previous ASignal value for the rotary encoder
int previousCommand = 0;            // Keep track of previous command to avoid back-to-back repeats
int currentTrack = 10;               // Keep track of current track selected
int arrowCounter = 0;               // Keep track of relative arrow presses for track select

float timeBetweenTasks = 12.0;       // Time interval between commands becomes smaller with each successful attempt (seconds)

String currentDir = "";             // Variable for direction of steering wheel
String gameTracks[6] = {"Freestyle", "Rainbow Road", "Moo Moo Meadows", "Bowser's Castle", "Baby Park", "Coconut Mall"};

// Hard coded tracks for Mariokart
const int RainbowRoad[82] = {
    4, 2, 4, 2, 4, 2, 6, 3, 5, 6, 3, 5, 5, 4, 2, 4, 2, 6, 3, 1,
    4, 2, 6, 3, 5, 4, 2, 6, 3, 1, 6, 3, 1, 6, 3, 1, 4, 2, 4, 2,
    6, 3, 5, 4, 2, 6, 3, 1, 6, 3, 1, 4, 2, 6, 3, 5, 4, 2, 4, 2,
    6, 3, 5, 6, 3, 5, 1, 1, 6, 3, 1, 4, 2, 4, 2, 6, 3, 1, 4, 2,
    4, 2
};

const int MooMooMeadows[57] = {
    4, 2, 4, 2, 6, 3, 1, 1, 6, 3, 1, 2, 6, 3, 5, 2, 1, 4, 2, 1,
    3, 5, 6, 3, 1, 4, 2, 4, 2, 6, 3, 1, 4, 2, 4, 2, 4, 2, 6, 3,
    1, 4, 2, 4, 2, 6, 3, 5, 3, 1, 3, 5, 1, 4, 2, 4, 2
};

const int BowserCastle[77] = {
    4, 2, 4, 2, 6, 3, 5, 4, 2, 4, 2, 6, 3, 1, 4, 2, 6, 3, 5, 4,
    2, 6, 3, 5, 4, 2, 6, 3, 1, 4, 2, 6, 3, 1, 4, 2, 6, 3, 5, 4,
    2, 6, 3, 1, 4, 2, 6, 3, 1, 4, 2, 6, 3, 5, 4, 2, 4, 2, 6, 3,
    1, 6, 3, 1, 4, 2, 4, 2, 4, 2, 6, 3, 5, 4, 2, 4, 2
};

const int BabyPark[26] = {
    4, 2, 4, 2, 6, 3, 1, 6, 3, 1, 
    4, 2, 4, 2, 4, 2, 6, 3, 1, 6, 
    3, 1, 4, 2, 4, 2
};


void setup() {

  // Debugging
  Serial.begin(9600);

  // Define Pin Modes of All Sensors
  pinMode(PotentiometerGasPin, INPUT);
  pinMode(PotentiometerBrakePin, INPUT);
  pinMode(limitSwitchTopPin, INPUT_PULLUP);
  pinMode(limitSwitchBottomPin, INPUT_PULLUP);
  pinMode(ASignal,INPUT_PULLUP);
	pinMode(BSignal,INPUT_PULLUP);

  // Define Pin Modes for Buttons for Track Select
  pinMode(RightArrowSelect, INPUT_PULLUP);
  pinMode(LeftArrowSelect, INPUT_PULLUP);

  // Define Pin Modes for Other Buttons
  pinMode(startGameButton, INPUT);
  
  mp3.initialize();

  lcd.init();       // Initialize the LCD
  lcd.backlight();  // Turn on the backlight
  lcd.setCursor(0, 0);  // Set cursor to first row, first column

  // Set the brightness to 5 (0=dimmest 7=brightest)
	display.setBrightness(5);

	// Set all segments ON
	display.setSegments(allON);

	display.clear();

  display.showNumberDec(0);


	// Read the initial state of A Signal (for Rotary Encoder)
  lastA = digitalRead(ASignal);

  attachInterrupt(digitalPinToInterrupt(ASignal), handle_A, CHANGE);
  attachInterrupt(digitalPinToInterrupt(BSignal), handle_A, CHANGE);

  randomSeed(analogRead(A5));
}

void loop() {

  // previousStartButtonStatus = true -> means that the button is in track select mode
  // previousStartButtonStatus = false -> means that the button is in start game mode

  int gameStart = analogRead(startGameButton); // Read analog value from the button
  // Serial.println(analogRead(startGameButton));

  if (gameStart == 0) {
      startgame = true;
      previousStartButtonStatus = true;
  }

  if (startgame && correctCommand) {
  //if (true) {
    // START LOGIC HERE

    // ALL LOGIC FOR INDIVIDUAL SENSORS IS ALREADY CODED AND ATTACHED IN THE SCREENSHOTS BELOW

    // Game starts off by randomly picking a user command

    // Randomly generate a task for the user (1-3)
    // MAKE SURE TO HAVE THE SD CARD HAVE THE INSTRUCTIONS IN THIS ORDER
    // 1 = Turn Left
    // 2 = Gas It
    // 3 = Brake It
    // 4 = Shift Up
    // 5 = Turn Right
    // 6 = Shift Down

    // Track Select Logic -> Rainbow Road, Moo Moo Meadows, Bowser's Castle, Baby Park, Coconut Mall
    // Display track selection title
    if (analogRead(startGameButton) != 0) {
      while (previousStartButtonStatus) {  // Keep selecting tracks while start button is not pressed
          lcd.setCursor(0, 0);
          lcd.print("Select Track:");

          // Read button states
          bool rightPressed = digitalRead(RightArrowSelect) == HIGH;
          bool leftPressed = digitalRead(LeftArrowSelect) == HIGH;

          // Right arrow logic (increment only if not at last track)
          if (rightPressed && !previousRightState && arrowCounter < 5) {
              arrowCounter++;
          }

          // Left arrow logic (decrement only if not at first track)
          if (leftPressed && !previousLeftState && arrowCounter > 0) {
              arrowCounter--;
          }

          // Store previous button states to detect new presses
          previousRightState = rightPressed;
          previousLeftState = leftPressed;

          // Update LCD with the selected track in the second row
          lcd.setCursor(0, 1);
          lcd.print("                "); // Clear previous track
          lcd.setCursor(0, 1);
          lcd.print(gameTracks[arrowCounter]);  // Display new track

          // Check if start button is pressed (assuming analog input reads 0 when pressed)
          if (analogRead(startGameButton) == 0) {
            currentTrack = arrowCounter;
            previousStartButtonStatus = false;  // Exit track selection loop
          }

          delay(20);  // Small debounce delay
      }
    }

    if (!previousStartButtonStatus) {

        if (currentTrack == 1) {
          currentTask = RainbowRoad[totalPoints];
          Serial.print("Perform Task: ");
          Serial.println(currentTask);
        } else if (currentTrack == 2) {
          currentTask = MooMooMeadows[totalPoints];
          Serial.print("Perform Task: ");
          Serial.println(currentTask);
        } else if (currentTrack == 3) {
          currentTask = BowserCastle[totalPoints];
          Serial.print("Perform Task: ");
          Serial.println(currentTask);
        } else if (currentTrack == 4) {
          currentTask = BabyPark[totalPoints];
          Serial.print("Perform Task: ");
          Serial.println(currentTask);
        } else if (currentTrack == 0) {
          currentTask = random(1, 7);
          Serial.print("Perform Task: ");
          Serial.println(currentTask);
        }

        Serial.println("Current Track");
        Serial.println(currentTrack);

        // currentTask = random(1, 7);
        // Serial.print("Perform Task: ");
        // Serial.println(currentTask);

        while (currentTask == previousCommand) {
          currentTask = random(1, 7);
          Serial.print("Changed Repeated Task To: ");
          Serial.println(currentTask);
        }

        lcd.clear();

        if (currentTask == 1) {
          lcd.print("Turn Left!");
        } else if (currentTask == 2) {
          lcd.print("Gas It!");
        } else if (currentTask == 3) {
          lcd.print("Brake It!");
        } else if (currentTask == 4) {
          lcd.print("Shift Up!");
        } else if (currentTask == 5) {
          lcd.print("Turn Right!");
        } else if (currentTask == 6) {
          lcd.print("Shift Down!");
        }

        // Serial.println("Playing Music Now");

        mp3.playTrackNumber(currentTask, 15);  // Play the track

        // Start a timer for user response
        long startTime = millis();
        bool taskCompleted = false;

        while (millis() - startTime < timeBetweenTasks * 1000) {
          // 1 = Bad Response
          // 2 = Correct Response
          // 3 = No Response Yet
          int goodResponse = checkUserResponse(currentTask);
            if (goodResponse == 2) {
                taskCompleted = true;
                Serial.println("User Passed Current Command!");
                delay(2000);
                goodResponse = 0;
                break;
            } else if (goodResponse == 1) {
              taskCompleted = false;
              Serial.println("Incorrect Command Done by User!");
              lcd.clear();
              lcd.print("Game Over!");
              delay(2000);
              goodResponse = 0;
              break;
            }
        }

        if (taskCompleted) {
          if (pointsDoubleActive and pointsDoubleCounter >= 0) {
            if (pointsDoubleCounter == 0) {
              pointsDoubleActive = false;
              lcd.clear();
              lcd.setCursor(0, 0);
              if (currentTask == 1) {
                lcd.print("Turn Left!");
              } else if (currentTask == 2) {
                lcd.print("Gas It!");
              } else if (currentTask == 3) {
                lcd.print("Brake It!");
              } else if (currentTask == 4) {
                lcd.print("Shift Up!");
              } else if (currentTask == 5) {
                lcd.print("Turn Right!");
              } else if (currentTask == 6) {
                lcd.print("Shift Down!");
              }
            } else if (pointsDoubleCounter > 0) {
              totalPoints += 2;
              pointsDoubleCounter--;
            }
          } else {
            totalPoints++;
          }
          display.showNumberDec(totalPoints);
          Serial.print("Points: ");
          Serial.println(totalPoints);

          // Reduce the time between tasks only if the time freeze power up is not active
            if (!timeFreezeActive) {
              timeBetweenTasks -= 0.1;
              if (currentTask == 2) {
                timeBetweenTasks -= 0.1;
              } else if (currentTask == 3) {
                timeBetweenTasks += 0.2;
              }
            } else if (timeFreezeActive and timeFreezeCounter >= 0) {
              if (timeFreezeCounter == 0) {
                timeFreezeActive = false;
                lcd.clear();
                lcd.setCursor(0, 0);
                if (currentTask == 1) {
                  lcd.print("Turn Left!");
                } else if (currentTask == 2) {
                  lcd.print("Gas It!");
                } else if (currentTask == 3) {
                  lcd.print("Brake It!");
                } else if (currentTask == 4) {
                  lcd.print("Shift Up!");
                } else if (currentTask == 5) {
                  lcd.print("Turn Right!");
                } else if (currentTask == 6) {
                  lcd.print("Shift Down!");
                }
              } else if (timeFreezeCounter > 0) {
                timeBetweenTasks -= 0.0;
              }
            }

            // Randomly generate power-ups
            applyPowerUp();

          correctCommand = true;

        } else if (!taskCompleted) {
          // Game ends if the user failed to complete the task on time
          Serial.println("Game Over! Final Score: " + String(totalPoints));
          lcd.clear();
          lcd.print("Game Over!");
          correctCommand = false;
          startgame = false;
        }

        if (totalPoints == 99 && currentTrack == 0) {
          Serial.println("Congratulations! You won the game!");
          startgame = false;
          totalPoints = 0;
        } else if (totalPoints == 82 && currentTrack == 1) {
          Serial.println("Congratulations! You won the game!");
          startgame = false;
          totalPoints = 0;
        } else if (totalPoints == 57 && currentTrack == 2) {
          Serial.println("Congratulations! You won the game!");
          startgame = false;
          totalPoints = 0;
        } else if (totalPoints == 77 && currentTrack == 3) {
          Serial.println("Congratulations! You won the game!");
          startgame = false;
          totalPoints = 0;
        } else if (totalPoints == 26 && currentTrack == 4) {
          Serial.println("Congratulations! You won the game!");
          startgame = false;
          totalPoints = 0;
        } else if (totalPoints == 44 && currentTrack == 5) {
          Serial.println("Congratulations! You won the game!");
          startgame = false;
          totalPoints = 0;
        }
      previousCommand = currentTask;
    }
  }


  delay(100);  // Delay

}

//*********************************
//*********** FUNCTIONS ***********
//*********************************


// Function to check if the user performed the correct response
int checkUserResponse(int task) {
    switch (task) {
      // 1 = Turn Left
      case 1:
        Serial.println("In 1");

        if (analogRead(PotentiometerGasPin) > 400) {
          Serial.println("Gas Instead");
          return 1;
        } else if (analogRead(PotentiometerBrakePin) > 400) {
          Serial.println("Brake Instead");
          return 1;
        } else if (digitalRead(limitSwitchTopPin) == LOW) {
          Serial.println("Shift Up Instead");
          return 1;
        } else if (digitalRead(limitSwitchBottomPin) == LOW) {
          Serial.println("Shift Down Instead");
          return 1;
        } else if (currentDir == "Clockwise") {
          Serial.println("Turn Right Instead");
          return 1;
        } else if (currentDir == "Counterclockwise") {
          return 2;
        } else {
          return 3;
        }
        break;
        
      // 2 = Gas It
      case 2:

        Serial.println("In 2");

        if (currentDir != "") {
          Serial.println("Turn Instead");
          return 1;
        } else if (analogRead(PotentiometerBrakePin) > 400) {
          Serial.println("Brake Instead");
          return 1;
        } else if (digitalRead(limitSwitchTopPin) == LOW) {
          Serial.println("Shift Up Instead");
          return 1;
        } else if (digitalRead(limitSwitchBottomPin) == LOW) {
          Serial.println("Shift Down Instead");
          return 1;
        } else if (currentDir != "") {
          Serial.println("Turn Instead");
          return 1;
        } else if (analogRead(PotentiometerGasPin) > 400) {
          return 2;
        } else {
          return 3;
        }
        break;

      // 3 = Brake It
      case 3: 
        Serial.println("In 3");

        if (currentDir == "Counterclockwise") {
          Serial.println("Turn Left Instead");
          return 1;
        } else if (analogRead(PotentiometerGasPin) > 400) {
          Serial.println("Gas Instead");
          return 1;
        } else if (digitalRead(limitSwitchTopPin) == LOW) {
          Serial.println("Shift Up Instead");
          return 1;
        } else if (digitalRead(limitSwitchBottomPin) == LOW) {
          Serial.println("Shift Down Instead");
          return 1;
        } else if (currentDir == "Clockwise") {
          Serial.println("Turn Right Instead");
          return 1;
        } else if (analogRead(PotentiometerBrakePin) > 400) {
          return 2;
        } else {
          return 3;
        }
        break;
      
      // 4 = Shift Up
      case 4:

        Serial.println("In 4");

        if (currentDir == "Counterclockwise") {
          Serial.println("Turn Left Instead");
          return 1;
        } else if (analogRead(PotentiometerGasPin) > 400) {
          Serial.println("Gas Instead");
          return 1;
        } else if (analogRead(PotentiometerBrakePin) > 400) {
          Serial.println("Brake Instead");
          return 1;
        } else if (digitalRead(limitSwitchBottomPin) == LOW) {
          Serial.println("Shift Down Instead");
          return 1;
        } else if (currentDir == "Clockwise") {
          Serial.println("Turn Right Instead");
          return 1;
        } else if (digitalRead(limitSwitchTopPin) == LOW) {
          return 2;
        } else {
          return 3;
        }
        break;
        
      // 5 = Turn Right
      case 5:

        Serial.println("In 5");

        if (analogRead(PotentiometerGasPin) > 400) {
          Serial.println("Gas Instead");
          return 1;
        } else if (analogRead(PotentiometerBrakePin) > 400) {
          Serial.println("Brake Instead");
          return 1;
        } else if (digitalRead(limitSwitchTopPin) == LOW) {
          Serial.println("Shift Up Instead");
          return 1;
        } else if (digitalRead(limitSwitchBottomPin) == LOW) {
          Serial.println("Shift Down Instead");
          return 1;
        } else if (currentDir == "Counterclockwise") {
          Serial.println("Left Instead");
          return 1;
        } else if (currentDir == "Clockwise") {
          return 2;
        } else {
          return 3;
        }
        break;

      // 6 = Shift Down
      case 6:

        Serial.println("In 6");

        if (currentDir == "Counterclockwise") {
          return 1;
        } else if (analogRead(PotentiometerGasPin) > 400) {
          Serial.println("Gas Instead");
          return 1;
        } else if (analogRead(PotentiometerBrakePin) > 400) {
          Serial.println("Brake Instead");
          return 1;
        } else if (digitalRead(limitSwitchTopPin) == LOW) {
          Serial.println("Shift Up Instead");
          return 1;
        } else if (currentDir == "Clockwise") {
          Serial.println("Turn Right Instead");
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

// Function to randomly generate power-ups
void applyPowerUp() {
    int powerUpChance = random(1, 11);

    switch (powerUpChance) {
        // Bonus five points for one time
        case 1:
            Serial.println("Power-Up: 5 Bonus Points!");
            totalPoints += 5;
            display.showNumberDec(totalPoints);
            Serial.print("Points: ");
            Serial.println(totalPoints);
            lcd.setCursor(0, 1);  // Set cursor to first row, first column
            lcd.print("Bonus Five Points!");
            break;
        // Points Double for five commands
        case 2:
            if (!pointsDoubleActive) {
              Serial.println("Power-Up: Double Points for 5 Seconds!");
              pointsDoubleCounter = 5;
              pointsDoubleActive = true;
              lcd.setCursor(0, 1);  // Set cursor to first row, first column
              lcd.print("x2 Points for Five Commands!");
              break;
            }
        // Time freeze for five commands
        case 3:
          if (!timeFreezeActive) {
            Serial.println("Power-Up: Time Freeze for 5 Seconds!");
            timeFreezeCounter = 5;
            timeFreezeActive = true;
            lcd.setCursor(0, 1);  // Set cursor to first row, first column
            lcd.print("Time Freeze for Five Commands!");
            break;
          }
        default:
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
      Serial.print("Encoder Counter: ");
      Serial.println(counter);  
    }


    // Positive Counter -> Clockwise
    if (counter > 300) {
      currentDir = "Clockwise";
    } else if (counter < -300) {
      currentDir = "Counterclockwise";
    } else {
      currentDir = "";
    }
  }
}
