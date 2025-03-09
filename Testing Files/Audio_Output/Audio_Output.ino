// #include <SoftwareSerial.h>
#include <AltSoftSerial.h>
#include <DFRobotDFPlayerMini.h>

const int RXPin = 8;
const int TXPin = 9;

AltSoftSerial mySerial;  // Uses fixed pins (TX=9, RX=8)
DFRobotDFPlayerMini player;                   // Making an instance of the DFPlayer Mini


void setup() {
    Serial.begin(9600);
    // softwareSerial.begin(9600);

    mySerial.begin(9600);

    // Setting up DFPlayer for Audio
    if (!player.begin(mySerial)) {
      Serial.println("DFPlayer Mini not detected!");
      while (true);
    }

    player.volume(20); // Set volume to 20 (out of 30)
}

void loop() {
    int track = random(1, 4);
    
    player.play(track);
}
