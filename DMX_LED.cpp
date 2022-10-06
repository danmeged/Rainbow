// DMX_LED.cpp
#include "DMX_LED.h"
#include <DMXSerial.h>
#include <FastLED.h>

DMX_LED::DMX_LED(int myRedChannel, int myGreenChannel, int myBlueChannel, CRGB initialColor) {
  redChannel = myRedChannel;
  greenChannel = myGreenChannel;
  blueChannel = myBlueChannel;

  redVal = initialColor.red;
  newRedVal = initialColor.red;

  greenVal = initialColor.green;
  newGreenVal = initialColor.green;

  blueVal =  initialColor.blue;
  newBlueVal =  initialColor.blue;

  newColor = initialColor;
  
  DMXSerial.write(redChannel, redVal);
  DMXSerial.write(greenChannel, greenVal);
  DMXSerial.write(blueChannel, blueVal);
}

void DMX_LED::setNewColor(CRGB myNewColor) {
  newColor = myNewColor;
  newRedVal = myNewColor.red;
  newGreenVal = myNewColor.green;
  newBlueVal = myNewColor.blue;
}

void DMX_LED::blendColor() {
  
  if (redVal < newRedVal) {
    redVal++;
  }
  else if (redVal > newRedVal) {
    redVal--;
  }
  DMXSerial.write(redChannel, redVal);

  if (greenVal < newGreenVal) {
    greenVal++;
  }
  else if (greenVal > newGreenVal) {
    greenVal--;
  }
  DMXSerial.write(greenChannel, greenVal);

  if (blueVal < newBlueVal) {
    blueVal++;
  }
  else if (blueVal > newBlueVal) {
    blueVal--;
  }
  DMXSerial.write(blueChannel, blueVal);

}

bool DMX_LED::isSameColor(CRGB color) {
  return (color.red == newColor.red && color.green == newColor.green && color.blue == newColor.blue);
}

void DMX_LED::setColor(CRGB color) {
  DMXSerial.write(redChannel, color.red);
  redVal = color.red;
  newRedVal = color.red;

  DMXSerial.write(greenChannel, color.green);
  greenVal = color.green;
  newGreenVal = color.green;

  DMXSerial.write(blueChannel, color.blue);
  blueVal =  color.blue;
  newBlueVal =  color.blue;

  newColor = color;
}