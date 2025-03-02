// TEST THIS LOGIC OUT WITH AN LED OUTPUT

const int magneticSensorPin = 3;
const int LEDPin = 4;

void setup() {
    pinMode(magneticSensorPin, INPUT_PULLUP);
    pinMode(LEDPin, OUTPUT);

    Serial.begin(9600);
}

void loop() {
    if (digitalRead(magneticSensorPin) == LOW) {
        Serial.println("Magnet is in front of Sensor");
        digitalWrite(LEDPin, HIGH);
    } else {
        Serial.println("Magnet is not in front of Sensor");
        digitalWrite(LEDPin, LOW);
    }
}
