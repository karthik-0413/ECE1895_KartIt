#include <avr/interrupt.h>

const int A_Signal = 2;
const int B_Signal = 3;

int counter = 0;
String currentDir = "";
int lastA = LOW;

void setup() {

	// Set encoder pins as inputs
    pinMode(A_Signal, INPUT_PULLUP);
    pinMode(B_Signal, INPUT_PULLUP);
    pinMode(8, OUTPUT);

	// Setup Serial Monitor
    Serial.begin(9600);

	// Read the initial state of A Signal
    lastA = digitalRead(A_Signal);

    // Enable Pin Change Interrupts (PCINT) for Pins 11 and 12
    // Enable Pin Change Interrupts (PCINT) for Pins 4 and 5
    // PCICR |= (1 << PCIE0);  		// Enable PCINT for PORT B (pins 8-13)
    // PCICR |= B00000100;      // Enable PCINT for PORT D (pins 4-13)
    // PCMSK0 |= (1 << PCINT5); 		// Enable interrupt for pin 11 (PB5)
    // PCMSK2 |= B00100000 | B00010000; 		// Enable interrupt for pin 11 (PD4) -> PCINT20/PCINT21
    // PCMSK0 |= (1 << PCINT4); 		// Enable interrupt for pin 12 (PB4)
    // PCMSK0 |= (1 << PCINT5); 		// Enable interrupt for pin 12 (PD5)

    attachInterrupt(digitalPinToInterrupt(A_Signal), handle_A, CHANGE);
    attachInterrupt(digitalPinToInterrupt(B_Signal), handle_A, CHANGE);
    
}

void handle_A() {
    int currentA = digitalRead(A_Signal);
    int currentB = digitalRead(B_Signal);

    if (currentA != lastA) {  // Detect change in A_Signal
        if (currentA == HIGH) {
            if (currentB == LOW) {
                counter--;
                currentDir = "CounterClockwise";
                digitalWrite(4, LOW);
            } else {
                counter++;
                currentDir = "clockwise";
                digitalWrite(4, HIGH);
            }
        } else {
            if (currentB == LOW) {
                counter++;
                currentDir = "clockwise";
                digitalWrite(4, HIGH);
            } else {
                counter--;
                currentDir = "Counterwise";
                digitalWrite(4, LOW);
            }
        }
        lastA = currentA;
    }
}

void loop() {
    static int lastCounter = 0;
    
	// When steering wheel has been moved
    if (counter != lastCounter) {
        Serial.print("Encoder turned ");
        Serial.print(currentDir);
        Serial.print(" | Counter: ");
        Serial.println(counter);
        lastCounter = counter;
    }
    
    delay(5);
}
