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
  // newRedVal = newColor.red;
  // newGreenVal = newColor.green;
  // newBlueVal = newColor.blue;
  newRedVal = myNewColor.red;
  newGreenVal = myNewColor.green;
  newBlueVal = myNewColor.blue;
}

void DMX_LED::blendColor() {
  
  // Serial.println("test1 - redVal: "+String(redVal)+" || greenVal: "+String(greenVal)+" || blueVal: "+String(blueVal));
  // Serial.println("test2 - newRedVal: "+String(newRedVal)+" || newGreenVal: "+String(newGreenVal)+" || newBlueVal: "+String(newBlueVal));

  if (redVal < newRedVal) {
    redVal++;
  }
  else if (redVal > newRedVal) {
    redVal--;
  }
  if (greenVal < newGreenVal) {
    greenVal++;
  }
  else if (greenVal > newGreenVal) {
    greenVal--;
  }
  if (blueVal < newBlueVal) {
    blueVal++;
  }
  else if (blueVal > newBlueVal) {
    blueVal--;
  }
  // DmxSimple.maxChannel(0);
  DMXSerial.write(redChannel, redVal);
  DMXSerial.write(greenChannel, greenVal);
  DMXSerial.write(blueChannel, blueVal);
          
  // DmxSimple.write(13, redVal);
  // DmxSimple.write(14, greenVal);
  // DmxSimple.write(15, blueVal);

  // DmxSimple.write(16, redVal);
  // DmxSimple.write(17, greenVal);
  // DmxSimple.write(18, blueVal);
}

bool DMX_LED::isSameColor(CRGB color) {
  return (color.red == newColor.red && color.green == newColor.green && color.blue == newColor.blue);
}