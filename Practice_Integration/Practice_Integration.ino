// <#include <SoftwareSerial.h>
#include "mp3tf16p.h"
#include <avr/interrupt.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
// #include <TM1637.h>

// Display:
// Seven Segment -> Score
// LCD -> Commands

// Initialize Sensor Pins Here
// const int RXPin = 0;                   // Audio Output Pin
// const int TXPin = 1;                   // Audio Output Pin
const int ASignal = 2;                  // Rotary Encoder for Steering
const int BSignal = 3;                    // Rotary Encoder for Steering
const int ConfirmButton = 4;           // Button for Confirming User Input
const int startGameButton = 5;          // System Requirements
const int limitSwitchTopPin = 6;     // Hall Effect Sensor
const int limitSwitchBottomPin = 7;  // Hall Effect Sensor
const int JoystickButtonPin = 8;      // Joystick Button
const int TMLCLK = 9;                  // Seven Segment Display Clock
const int TMDATA = 10;                  // Seven Segment Display Data
const int RXPin = 11;
const int TXPin = 12;
const int PotentiometerGasPin = A0;        // Limit Switch
const int PotentiometerBrakePin = A1;      // Limit Switch
const int JoystickXPin = A2;               // Joystick X-Axis


// 13, A3, A4, A5



// Setting up DFPlayer Mini
// SoftwareSerial softwareSerial(RXPin, TXPin);  // Setting up DFPlayer Mini
MP3Player mp3(RXPin, TXPin);

// Define the LCD address (default is 0x27 or 0x3F, depending on your module)
LiquidCrystal_I2C lcd(0x27, 16, 2); // 16x2 LCD

// Seven Segment Display
// TM1637 tm(TMLCLK, TMDATA);


// Variables for Logic
bool systemOn = false;              // System On Flag
bool startgame = false;              // Game Start Flag
bool lostGame = false;              // If the user is unsuccessful in responding to a command either by not responding in time or by providing an incorrect response
bool correctCommand = true;         // Flag to keep track if the user did the right command
bool timeFreezeActive = false;      // Flag to keep track of powerup being activated
bool pointsDoubleActive = false;    // Flag to keep track of powerup being activated

int totalPoints = 0;                // Points Tracker
int currentTask = 1;                // Random Int Function picks number between 1-3 for Command
int timeFreezeCounter = 5;         // 5 seconds of no timer for player
int pointsDoubleCounter = 5;       // 5 seconds of double points for player
int counter = 0;                    // Counter variable for the rotary encoder
int finalCounter = 0;
int lastA = LOW;                    // Keep track of previous ASignal value for the rotary encoder
int previousCommand = 0;            // Keep track of previous command to avoid back-to-back repeats

float timeBetweenTasks = 12.0;       // Time interval between commands becomes smaller with each successful attempt (seconds)

String currentDir = "";             // Variable for direction of steering wheel



void setup() {

  // Define Pin Modes of All Sensors
  pinMode(PotentiometerGasPin, INPUT);
  pinMode(PotentiometerBrakePin, INPUT);
  pinMode(limitSwitchTopPin, INPUT_PULLUP);
  pinMode(limitSwitchBottomPin, INPUT_PULLUP);
  pinMode(ASignal,INPUT_PULLUP);
	pinMode(BSignal,INPUT_PULLUP);
  pinMode(LEDPin, OUTPUT);

  // Define Pin Modes for Other Buttons
  pinMode(startGameButton, INPUT_PULLUP);


  // Debugging
  Serial.begin(9600);
  mp3.initialize();

  lcd.init();       // Initialize the LCD
  lcd.backlight();  // Turn on the backlight
  lcd.setCursor(0, 0);  // Set cursor to first row, first column

  // tm.begin();


	// Read the initial state of A Signal (for Rotary Encoder)
  lastA = digitalRead(ASignal);

  attachInterrupt(digitalPinToInterrupt(ASignal), handle_A, CHANGE);
  attachInterrupt(digitalPinToInterrupt(BSignal), handle_A, CHANGE);

  randomSeed(analogRead(A5));

  // // Enable Pin Change Interrupts for PCINT4 (Pin 4) and PCINT5 (Pin 5)
  // PCICR |= B00000010;      // Enable PCINT for PORT D (pins 4-13)
  // PCMSK1 |= B00000010 | B00000001; 		// Enable interrupt for pin 11 (PD4) -> PCINT20/PCINT21
    
}

void loop() {



  int gameStart = digitalRead(startGameButton); // 0 = Off ; 1 = On

  if (gameStart == LOW) {
      startgame = true;
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
      currentTask = random(1, 7);
      Serial.print("Perform Task: ");
      Serial.println(currentTask);

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

      mp3.playTrackNumber(currentTask, 20);  // Play the track


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
              Serial.print("User Passed Current Command!");
              delay(500);
              break;
          } else if (goodResponse == 1) {
            taskCompleted = false;
            Serial.print("Incorrect Command Done by User!");
            delay(500);
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
        // tm.display(totalPoints);
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
        correctCommand = false;
        startgame = false;
      }

      if (totalPoints >= 99) {
        Serial.println("Congratulations! You won the game!");
        startgame = false;
        totalPoints = 0;
      }

    previousCommand = currentTask;
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

          if (analogRead(PotentiometerGasPin) > 400) {
            return 1;
          } else if (analogRead(PotentiometerBrakePin) > 400) {
            return 1;
          } else if (digitalRead(limitSwitchTopPin) == LOW) {
            return 1;
          } else if (digitalRead(limitSwitchBottomPin) == LOW) {
            return 1;
          } else if (currentDir == "Clockwise") {
            currentDir = "";
            return 1;
          } else if (currentDir == "Counterclockwise") {
            currentDir = "";
            return 2;
          }
          
        // 2 = Gas It
        case 2:

          if (currentDir == "Counterclockwise") {
            currentDir = "";
            return 1;
          } else if (analogRead(PotentiometerBrakePin) > 400) {
            return 1;
          } else if (digitalRead(limitSwitchTopPin) == LOW) {
            return 1;
          } else if (digitalRead(limitSwitchBottomPin) == LOW) {
            return 1;
          } else if (currentDir == "Clockwise") {
            currentDir = "";
            return 1;
          } else if (analogRead(PotentiometerGasPin) > 400) {
            return 2;
          }

        // 3 = Brake It
        case 3: 

          if (currentDir == "Counterclockwise") {
            currentDir = "";
            return 1;
          } else if (analogRead(PotentiometerGasPin) > 400) {
            return 1;
          } else if (digitalRead(limitSwitchTopPin) == LOW) {
            return 1;
          } else if (digitalRead(limitSwitchBottomPin) == LOW) {
            return 1;
          } else if (currentDir == "Clockwise") {
            currentDir = "";
            return 1;
          } else if (analogRead(PotentiometerBrakePin) > 400) {
            return 2;
          }
        
        // 4 = Shift Up
        case 4:

          if (currentDir == "Counterclockwise") {
            currentDir = "";
            return 1;
          } else if (analogRead(PotentiometerGasPin) > 400) {
            return 1;
          } else if (analogRead(PotentiometerBrakePin) > 400) {
            return 1;
          } else if (digitalRead(limitSwitchBottomPin) == LOW) {
            return 1;
          } else if (currentDir == "Clockwise") {
            currentDir = "";
            return 1;
          } else if (digitalRead(limitSwitchTopPin) == LOW) {
            return 2;
          }
          
        // 5 = Turn Right
        case 5:

          if (analogRead(PotentiometerGasPin) > 400) {
            return 1;
          } else if (analogRead(PotentiometerBrakePin) > 400) {
            return 1;
          } else if (digitalRead(limitSwitchTopPin) == LOW) {
            return 1;
          } else if (digitalRead(limitSwitchBottomPin) == LOW) {
            return 1;
          } else if (currentDir == "Counterclockwise") {
            currentDir = "";
            return 1;
          } else if (currentDir == "Clockwise") {
            currentDir = "";
            return 2;
          }

        // 6 = Shift Down
        case 6:

          if (currentDir == "Counterclockwise") {
            currentDir = "";
            return 1;
          } else if (analogRead(PotentiometerGasPin) > 400) {
            return 1;
          } else if (analogRead(PotentiometerBrakePin) > 400) {
            return 1;
          } else if (digitalRead(limitSwitchTopPin) == LOW) {
            return 1;
          } else if (currentDir == "Clockwise") {
            currentDir = "";
            return 1;
          } else if (digitalRead(limitSwitchBottomPin) == LOW) {
            return 2;
          }

        default: 
          return 3;
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
            // tm.display(totalPoints);
            Serial.print("Points: ");
            Serial.println(totalPoints);
            lcd.setCursor(1, 0);  // Set cursor to first row, first column
            lcd.print("Bonus Five Points!");
            break;
        // Points Double for five commands
        case 2:
            if (!pointsDoubleActive) {
              Serial.println("Power-Up: Double Points for 5 Seconds!");
              pointsDoubleCounter = 5;
              pointsDoubleActive = true;
              lcd.setCursor(1, 0);  // Set cursor to first row, first column
              lcd.print("x2 Points for Five Commands!");
              break;
            }
        // Time freeze for five commands
        case 3:
          if (!timeFreezeActive) {
            Serial.println("Power-Up: Time Freeze for 5 Seconds!");
            timeFreezeCounter = 5;
            timeFreezeActive = true;
            lcd.setCursor(1, 0);  // Set cursor to first row, first column
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

      // Positive Counter -> Clockwise
      if (counter > 100) {
        currentDir = "Clockwise";
      } else if (counter < -100) {
        currentDir = "Counterclockwise";
      } else {
        currentDir = "";
      }

      
  }
}
