// Song.h
#ifndef Song_h
#define Song_h

#include <Arduino.h>

class Song {
  private:
    int bpm;

  public:
    Song(int myBPM);
    int getBPM();
};

#endif