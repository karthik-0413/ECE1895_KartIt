#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

const int RXPin = 10;
const int TXPin = 11;

SoftwareSerial softwareSerial(RXPin, TXPin);
DFRobotDFPlayerMini player;


void setup() {
    Serial.begin(9600);
    softwareSerial.begin(9600);

    if (player.begin(softwareSerial)) {
      Serial.println("OK");
    } else {
      Serial.println("FAIL!");
    }

    player.volume(20); // Set volume to 20 (out of 30)
}

void loop() {
    int track = random(1, 4);
    
    player.play(track);
}
