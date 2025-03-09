#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

// Initialize Sensor Pins Here
const int limitSwitchGasPin = 0;        // Limit Switch
const int limitSwitchBrakePin = 1;      // Limit Switch
const int magneticSensorTopPin = 2;     // Hall Effect Sensor
const int magneticSensorBottomPin = 3;  // Hall Effect Sensor
const int ASignal = 4;                 // Rotary Encoder for Steering
const int BSignal = 5;                    // Rotary Encoder for Steering
const int systemOnButton = 6;           // System Requirements
const int startGameButton = 7;          // System Requirements
const int RXPin = 8;                   // Audio Output Pin
const int TXPin = 9;                   // Audio Output Pin


// Setting up DFPlayer Mini
SoftwareSerial softwareSerial(RXPin, TXPin);  // Setting up DFPlayer Mini
DFRobotDFPlayerMini player;                   // Making an instance of the DFPlayer Mini


// Variables for Logic
bool systemOn = false;              // System On Flag
bool startgame = false;              // Game Start Flag
bool lostGame = false;              // If the user is unsuccessful in responding to a command either by not responding in time or by providing an incorrect response
bool correctCommand = true;         // Flag to keep track if the user did the right command
bool timeFreezeActive = false;      // Flag to keep track of powerup being activated
bool pointsDoubleActive = false;    // Flag to keep track of powerup being activated

int totalPoints = 0;                // Points Tracker
int currentTask = 1;                // Random Int Function picks number between 1-3 for Command
int timeFreezeDuration = 5;         // 5 seconds of no timer for player
int pointsDoubleDuration = 5;       // 5 seconds of double points for player
int counter = 0;                    // Counter variable for the rotary encoder
int lastA = LOW;                    // Keep track of previous ASignal value for the rotary encoder

float timeBetweenTasks = 12.0;       // Time interval between commands becomes smaller with each successful attempt (seconds)

String currentDir = "";             // Variable for direction of steering wheel



void setup() {

  // Define Pin Modes of All Sensors
  pinMode(limitSwitchGasPin, INPUT_PULLUP);
  pinMode(limitSwitchBrakePin, INPUT_PULLUP);
  pinMode(magneticSensorTopPin, INPUT_PULLUP);
  pinMode(magneticSensorBottomPin, INPUT_PULLUP);
  pinMode(ASignal,INPUT);
	pinMode(BSignal,INPUT);

  // Define Pin Modes for Other Buttons
  pinMode(systemOnButton, INPUT);
  pinMode(startGameButton, INPUT);


  // Debugging
  Serial.begin(9600);
  softwareSerial.begin(9600);     // DFPlayer Mini communication via UART1 (pins 14 and 15)

  // Setting up DFPlayer for Audio
  if (player.begin(softwareSerial)) {
      Serial.println("OK");
    } else {
      Serial.println("FAIL!");
    }


	// Read the initial state of A Signal (for Rotary Encoder)
  lastA = digitalRead(ASignal);

  // Attach interrupts to pins 4 and 5 (which support interrupts on Arduino Nano)
  attachInterrupt(digitalPinToInterrupt(ASignal), handleA, CHANGE);  // Trigger on change (both rising and falling edge)
  attachInterrupt(digitalPinToInterrupt(BSignal), handleA, CHANGE);  // Trigger on change (both rising and falling edge)
    
}

// 'ISR' function for the Rotary Encoder to detect steering wheel turning and which direction
void handleA() {
    int currentA = digitalRead(ASignal);
    int currentB = digitalRead(BSignal);

    if (currentA != lastA) {  // Detect change in A_Signal
        if (currentA == HIGH) {
            if (currentB == LOW) {
                counter++;
                currentDir = "Clockwise";
            } else {
                counter--;
                currentDir = "Counterclockwise";
            }
        } else {
            if (currentB == LOW) {
                counter--;
                currentDir = "Counterclockwise";
            } else {
                counter++;
                currentDir = "Clockwise";
            }
        }
        lastA = currentA;
    }
}


void loop() {

  int systemMode = digitalRead(systemOnButton); // 0 = Off ; 1 = On
  int gameStart = digitalRead(startGameButton); // 0 = Off ; 1 = On

  if (systemMode == HIGH) {
    systemOn = true;
  }

  if (systemOn && gameStart == HIGH) {
      startgame = true;
  }

  if (startgame && correctCommand) {
    // START LOGIC HERE

    // ALL LOGIC FOR INDIVIDUAL SENSORS IS ALREADY CODED AND ATTACHED IN THE SCREENSHOTS BELOW

    // Game starts off by randomly picking a user command

    // Randomly generate a task for the user (1-3)
    // MAKE SURE TO HAVE THE SD CARD HAVE THE INSTRUCTIONS IN THIS ORDER
    // 1 = Gas It
    // 2 = Brake It
    // 3 = Shift It
    // 4 = Left It
    // 5 = Right It
      currentTask = random(1, 6);
      Serial.print("Perform Task: ");
      Serial.println(currentTask);

      // Start a timer for user response
      unsigned long startTime = millis();
      bool taskCompleted = false;

      while (millis() - startTime < timeBetweenTasks * 1000) {
          // Check if the user correctly performed the task
          if (checkUserResponse(currentTask)) {
              taskCompleted = true;
              break;
          }
      }

    // If the user does the command properly, then one points gets added to the total points variable
        // this is the case if the user provides a reponse ONLY to the command given and no other command and the proper command has to be done within the time limit
        // Set correctCommand = true here
        // totalPoints++;
        // Add all sensor checks that is in the other separate files
    // else if the user is unsuccessful in responding to a command either by not responding in time or by providing an incorrect response, then game will end and the user will be notified of their final score 
        // In this case, the user would have the click the start game button in order to play again
        // Make sure to check here that other commands are not done 
        // Set correctCommand = false here

    if (taskCompleted) {
          // Increase points if the user succeeded
          if (pointsDoubleActive) {
            totalPoints += 2;
          } else {
            totalPoints ++;
          }
          Serial.print("Points: ");
          Serial.println(totalPoints);
          
          // Reduce the time between tasks for difficulty progression only if the time freeze power up is not active
          if (!timeFreezeActive) {
            timeBetweenTasks -= 0.1;
          }

          // Randomly grant power-ups
          applyPowerUp();
      } else {
          // Game over if the user failed to complete the task
          Serial.println("Game Over! Final Score: " + String(totalPoints));
          correctCommand = false;
          startgame = false;
      }
      

    // After each command, the time interval between successive commands becomes smaller with each successful attempt
        // Make sure to lower the time interval here
        // timeBetweenTasks -= 0.1;


    // There will be a periodic timer implemented here as an interrupt that will provide the user with powerups
        // One of the powerups is to get 5 extra points (totalPoint += 5)
        // Another one of the powerups is to give the double points for 5 seconds (pointsDoubleActive = true)
        // The final powerup is to freeze to time for 5 seconds (timeFreezeActive = true)


    // If the total points of the user is 99, then the game ends and the user is notified of their score
    // Check for win condition (99 points)
      if (totalPoints >= 99) {
          Serial.println("Congratulations! You won the game!");
          startgame = false;
          totalPoints = 0;
      }
  }

  delay(100);  // Delay

}

// Function to check if the user performed the correct response
bool checkUserResponse(int task) {
    switch (task) {
        case 1: 
          return digitalRead(limitSwitchGasPin) == HIGH;
        case 2: 
          return digitalRead(limitSwitchBrakePin) == HIGH;
        case 3: 
          return digitalRead(magneticSensorTopPin) == HIGH || digitalRead(magneticSensorBottomPin) == HIGH;
        case 4:
          return currentDir == 'Counterclockwise';
        case 5:
          return currentDir == 'Clockwise';
        default: 
          return false;
    }
}

// Function to randomly apply power-ups
void applyPowerUp() {
    int powerUpChance = random(1, 11);  // 1 in 3 chance of getting a power-up

    switch (powerUpChance) {
        case 1:
            Serial.println("Power-Up: 5 Bonus Points!");
            totalPoints += 5;
            break;
        case 2:
            Serial.println("Power-Up: Double Points for 5 Seconds!");
            pointsDoubleActive = true;
            delay(pointsDoubleDuration * 1000);
            pointsDoubleActive = false;
            break;
        case 3:
            Serial.println("Power-Up: Time Freeze for 5 Seconds!");
            timeFreezeActive = true;
            delay(timeFreezeDuration * 1000);
            timeFreezeActive = false;
            break;
        default:
          break;
    }
}

























