// ▪ Everytime the user responds correctly, they score 1 point

// ▪ After a successful attempt, the device will randomly select and issue a new
// command.

// ▪ The time interval between successive commands becomes smaller with
// each successful attempt

// ▪ After 99 successful attempts, the game ends and the user is notified of their
// score

// ▪ If the If the user is unsuccessful in responding to a command either by not responding in
// time or by providing an incorrect response, then game will end and the user will be notified
// of their final score



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


// Variables for Logic
bool systemOn = false;              // System On Flag
bool startgame = false              // Game Start Flag
bool lostGame = false;              // If the user is unsuccessful in responding to a command either by not responding in time or by providing an incorrect response
int totalPoints = 0;                // Points Tracker
int currentTask = 1;                // Random Int Function picks number between 1-3 for Command
float timeBetweenTasks = 10.0       // Time interval between commands becomes smaller with each successful attempt

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

  if (startgame) {
    // START LOGIC HERE



  }



  delay(100);  // Delay to reduce flickering

}
