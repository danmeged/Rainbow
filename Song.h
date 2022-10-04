// Song.h
#ifndef Song_h
#define Song_h

#include <Arduino.h>

class Song {
  private:
    int bpm;
    int number;

  public:
    Song(int myBPM, int songNumber);
    int getBPM();
    int getTrackNumber();
};

#endif