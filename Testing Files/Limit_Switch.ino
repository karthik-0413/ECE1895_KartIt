// TEST THIS LOGIC OUT WITH AN LED OUTPUT

const int limitSwitchPin = 1;  // Define the pin for the limit switch
const int LEDPin = 2;


void setup() {
    pinMode(limitSwitchPin, INPUT_PULLUP);
    pinMode(LEDPin, OUTPUT);

    Serial.begin(9600);
}

void loop() {
    if (digitalRead(LIMIT_SWITCH_PIN) == LOW) {
        Serial.println("Limit Switch has been Pressed");
        digitalWrite(LEDPin, HIGH);
    } else {
        Serial.println("Limit Switch has been Released");
        digitalWrite(LEDPin, LOW);
    }
}
