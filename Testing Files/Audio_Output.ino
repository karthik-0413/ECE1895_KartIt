#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

const int RX = 10;
const int TX = 11;

SoftwareSerial softwareSerial(RX, TX);
DFRobotDFPlayerMini player;


void setup() {
    Serial.begin(9600);
    softwareSerial.begin(9600);

    if (player.begin(softwareSerial)) {
        Serial.println("OK");
    } else {
        Serial.println("FAIL!");
    }

    myDFPlayer.volume(20); // Set volume to 20 (out of 30)
    Serial.println("DFPlayer Mini Ready!");
}

void loop() {
    int track = random(1, 4);
    
    Serial.println(track);
    
    myDFPlayer.play(track);
}
