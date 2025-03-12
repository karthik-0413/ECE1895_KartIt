#include "mp3tf16p.h"

MP3Player mp3(8, 9);

int randomInt = 0;
bool playedOnce = false;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  mp3.initialize();
}

void loop() {

  if (!playedOnce) {
    randomInt = random(1, 3);
    mp3.playTrackNumber(randomInt, 10);
    mp3.serialPrintStatus(MP3_ALL_MESSAGE);
    playedOnce = true;
  }

}
