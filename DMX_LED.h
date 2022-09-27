// DMX_LED.h
#ifndef DMX_LED_h
#define DMX_LED_h

#include <Arduino.h>
#include <FastLED.h>

class DMX_LED {
  private:
    int redChannel;
    int greenChannel;
    int blueChannel;
    uint8_t redVal;
    uint8_t greenVal;
    uint8_t blueVal;
    uint8_t newRedVal;
    uint8_t newGreenVal;
    uint8_t newBlueVal;
    CRGB newColor;
    // CRGB color;



  public:
    DMX_LED(int myRedChannel, int myGreenChannel, int myBlueChannel, CRGB initialColor);
    
    void setColor(CRGB myNewColor);
    void blendColor();
    bool isSameColor(CRGB color);
};

#endif