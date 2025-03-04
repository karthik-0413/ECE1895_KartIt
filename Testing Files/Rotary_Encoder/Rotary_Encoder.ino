#include <avr/interrupt.h>

const int A_Signal = 11;
const int B_Signal = 12;

int counter = 0;
String currentDir = "";
int lastA = LOW;

void setup() {

	// Set encoder pins as inputs
    pinMode(A_Signal, INPUT);
    pinMode(B_Signal, INPUT);

	// Setup Serial Monitor
    Serial.begin(9600);

	// Read the initial state of A Signal
    lastA = digitalRead(A_Signal);

    // Enable Pin Change Interrupts (PCINT) for Pins 11 and 12
    PCICR |= (1 << PCIE0);  		// Enable PCINT for PORT B (pins 8-13)
    PCMSK0 |= (1 << PCINT5); 		// Enable interrupt for pin 11 (PB5)
    PCMSK0 |= (1 << PCINT4); 		// Enable interrupt for pin 12 (PB4)
}

ISR(PCINT0_vect) {
    int currentA = digitalRead(A_Signal);
    int currentB = digitalRead(B_Signal);

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
