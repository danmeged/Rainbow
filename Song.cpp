// Song.cpp
#include "Song.h"

Song::Song(int myBPM) {
  bpm = myBPM;
}

int Song::getBPM() {
  return bpm;
}