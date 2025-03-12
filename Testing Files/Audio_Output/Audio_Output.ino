#include <AltSoftSerial.h>
#include <DFRobotDFPlayerMini.h>

const int RXPin = 8;
const int TXPin = 9;

AltSoftSerial mySerial;  // Uses fixed pins (TX=9, RX=8)
DFRobotDFPlayerMini player;

void setup() {
    Serial.begin(9600);         // Start Serial Monitor
    mySerial.begin(9600);       // Start communication with DFPlayer Mini

    Serial.println("Initializing DFPlayer Mini...");

    if (!player.begin(mySerial)) {
        Serial.println("DFPlayer Mini initialization failed!");
        while (true); // Stop execution if DFPlayer doesn't initialize
    }

    Serial.println("DFPlayer Mini initialized successfully.");
    Serial.println("Sending play command...");

    player.volume(30);  // Set volume
    player.play(1);     // Play track 001 in folder 01

    Serial.println("If the DFPlayer is working, it should now be playing track 001.");
}

void loop() {
    // Read the DFPlayer's status
    if (player.available()) {
        int status = player.readType();
        int value = player.read();

        Serial.print("DFPlayer Status: ");
        Serial.print(status);
        Serial.print(" | Value: ");
        Serial.println(value);
    }
}
