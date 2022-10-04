// Song.cpp
#include "Song.h"

Song::Song(int myBPM, int songNumber) {
  bpm = myBPM;
  number = songNumber;
}

int Song::getBPM() {
  return bpm;
}

int Song::getTrackNumber() {
  return number;
}