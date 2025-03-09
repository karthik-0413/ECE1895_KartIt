const int buttonPin = 4;
const int ledPin = 13;

bool ledState = false;

void setup() {
    pinMode(ledPin, OUTPUT);
    pinMode(buttonPin, INPUT_PULLUP);

    attachInterrupt(digitalPinToInterrupt(buttonPin), toggleLED, FALLING);

    Serial.begin(9600);
}

void loop() {
    if (ledState) {
        digitalWrite(ledPin, HIGH);
    } else {
        digitalWrite(ledPin, LOW);
    }

    delay(500);
}

void toggleLED() {
    ledState = !ledState;
}
