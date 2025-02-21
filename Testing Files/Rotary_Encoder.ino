// TEST THIS LOGIC OUT LOOKING AT SERIAL MONITOR AND SEEING THE DEBUGGING OUTPUTS


const int clockPin = 11;
const int DTPin = 12;
const int switchPin = 13;

// Other needed variables
int counter = 0;
int currentStateCLK;
int lastStateCLK;
String currentDir ="";
unsigned long lastButtonPress = 0;

void setup() {
        
	// Set encoder pins as inputs
	pinMode(clockPin,INPUT);
	pinMode(DTPin,INPUT);
	pinMode(switchPin, INPUT_PULLUP);

	// Setup Serial Monitor
	Serial.begin(9600);

	// Read the initial state of CLK
	lastStateCLK = digitalRead(clockPin);
}

void loop() {
        
	// Read the current state of CLK
	currentStateCLK = digitalRead(clockPin);

	// If last and current state of CLK are different, then pulse occurred
	// React to only 1 state change to avoid double count
	if (currentStateCLK != lastStateCLK  && currentStateCLK == 1){

		// If the DT state is different than the CLK state then
		// the encoder is rotating CCW so decrement
		if (digitalRead(DTPin) != currentStateCLK) {
			counter ++;
			currentDir ="CW";
		} else {
			// Encoder is rotating CW so increment
			counter --;
			currentDir ="CCW";
		}

		Serial.print("Direction: ");
		Serial.print(currentDir);
		Serial.print(" | Counter: ");
		Serial.println(counter);
	}

	// Remember last CLK state
	lastStateCLK = currentStateCLK;

	// Read the button state
	int btnState = digitalRead(switchPin);

	//If we detect LOW signal, button is pressed
	if (btnState == LOW) {
	//if 50ms have passed since last LOW pulse, it means that the
		//button has been pressed, released and pressed again
		if (millis() - lastButtonPress > 50) {
			Serial.println("Button pressed!");
		}

		// Remember last button press event
		lastButtonPress = millis();
	}

	// Put in a slight delay to help debounce the reading
	delay(1);
}
