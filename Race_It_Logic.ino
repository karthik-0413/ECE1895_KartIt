
// Initialize Sensor Pins Here
const int limitSwitchGasPin = 1;        // Limit Switch
const int limitSwitchBrakePin = 2;      // Limit Switch
const int magneticSensorTopPin = 3;     // Hall Effect Sensor
const int magneticSensorBottomPin = 4;  // Hall Effect Sensor
const int clockPin = 5;                 // Rotary Encoder for Steering
const int DTPin = 6;                    // Rotary Encoder for Steering
const int switchPin = 7;                // Rotary Encoder for Steering
const int systemOnButton = 8;           // System Requirements
const int startGameButton = 9;          // System Requirements
const int RXPin = 11;                   // Audio Output Pin
const int TXPin = 12;                   // Audio Output Pin


// Setting up DFPlayer Mini
SoftwareSerial softwareSerial(RXPin, TXPin);  // Setting up DFPlayer Mini
DFRobotDFPlayerMini player;                   // Making an instance of the DFPlayer Mini


// Variables for Logic
bool systemOn = false;              // System On Flag
bool startgame = false              // Game Start Flag
bool lostGame = false;              // If the user is unsuccessful in responding to a command either by not responding in time or by providing an incorrect response
bool correctCommand = true;         // Flag to keep track if the user did the right command
bool timeFreezeActive = false;      // Flag to keep track of powerup being activated
bool pointsDoubleActive = false;    // Flag to keep track of powerup being activated

int totalPoints = 0;                // Points Tracker
int currentTask = 1;                // Random Int Function picks number between 1-3 for Command
int timeFreezeDuration = 5;         // 5 seconds of no timer for player
int pointsDoubleDuration = 5;       // 5 seconds of double points for player

float timeBetweenTasks = 12.0       // Time interval between commands becomes smaller with each successful attempt (seconds)



void setup() {

  // Define Pin Modes of All Sensors
  pinMode(limitSwitchGasPin, INPUT_PULLUP);
  pinMode(limitSwitchBrakePin, INPUT_PULLUP);
  pinMode(magneticSensorTopPin, INPUT_PULLUP);
  pinMode(magneticSensorBottomPin, INPUT_PULLUP);
  pinMode(clockPin,INPUT);
	pinMode(DTPin,INPUT);
	pinMode(switchPin, INPUT_PULLUP);

  // Define Pin Modes for Other Buttons
  pinMode(systemOnButton, INPUT)
  pinMode(startGameButton, INPUT)


  // Debugging
  Serial.begin(9600);


  // Read the initial state of CLK for Rotary Encoder
	lastStateCLK = digitalRead(clockPin);
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
        // One of the comnands


    // If the user does the command properly, then one points gets added to the total points variable
        // this is the case if the user provides a reponse ONLY to the command given and no other command and the proper command has to be done within the time limit
        // Set correctCommand = true here
        // totalPoints++;
        // Add all sensor checks that is in the other separate files
    // else if the user is unsuccessful in responding to a command either by not responding in time or by providing an incorrect response, then game will end and the user will be notified of their final score 
        // In this case, the user would have the click the start game button in order to play again
        // Make sure to check here that other commands are not done 
        // Set correctCommand = false here
      

    // After each command, the time interval between successive commands becomes smaller with each successful attempt
        // Make sure to lower the time interval here
        // timeBetweenTasks -= 0.1;


    // There will be a periodic timer implemented here as an interrupt that will provide the user with powerups
        // One of the powerups is to get 5 extra points (totalPoint += 5)
        // Another one of the powerups is to give the double points for 5 seconds (pointsDoubleActive = true)
        // The final powerup is to freeze to time for 5 seconds (timeFreezeActive = true)


    // If the total points of the user is 99, then the game ends and the user is notified of their score
    /* if (totalPoints == 99 || correctCommand == false) {
      gameStart = false;
      totalPoints = 0;
    }*/ 

  }

  delay(100);  // Delay

}