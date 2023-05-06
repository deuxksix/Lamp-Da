#include "wipes.h"

#include "constants.h"
#include "utils.h"

namespace animations
{

bool dotWipeDown(const uint32_t color, const uint32_t duration, const bool restart, Adafruit_NeoPixel& strip)
{
  static uint16_t targetIndex = 0;
  static unsigned long previousMillis = 0;
  const uint16_t numberOfLedSegments = strip.numPixels();

  // reset condition
  if (restart)
  {
    targetIndex = 0;
    previousMillis = 0;
    return false;
  }

  // finished if the target index is over the led limit
  if (targetIndex >= numberOfLedSegments)
    return true;

  // convert duration in delay for each segment
  const uint16_t delay = duration / (float)numberOfLedSegments;

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= delay and targetIndex < numberOfLedSegments) {
    previousMillis = currentMillis;

    strip.clear();
    strip.setPixelColor(targetIndex++, color);  //  Set pixel's color (in RAM)
    strip.show();  //  Update strip to match
  }

  return false;
}

bool dotWipeUp(const uint32_t color, const uint32_t duration, const bool restart, Adafruit_NeoPixel& strip)
{
  const uint16_t numberOfLedSegments = strip.numPixels();
  static uint16_t targetIndex = numberOfLedSegments - 1;
  static unsigned long previousMillis = 0;

  // reset condition
  if (restart)
  {
    targetIndex = numberOfLedSegments - 1;
    previousMillis = 0;
    return false;
  }

  // finished if the target index is over the led limit
  if (targetIndex >= numberOfLedSegments)
    return true;

  // convert duration in delay for each segment
  const unsigned long delay = duration / (float)numberOfLedSegments;

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= delay and targetIndex >= 0) {
    previousMillis = currentMillis;

    strip.clear();
    strip.setPixelColor(targetIndex--, color);  //  Set pixel's color (in RAM)
    strip.show();  //  Update strip to match
  }

  return false;
}

bool dotWipeDownRainbow(const uint32_t duration, const bool restart, Adafruit_NeoPixel& strip)
{
  static uint16_t targetIndex = 0;
  static unsigned long previousMillis = 0;
  const uint16_t numberOfLedSegments = strip.numPixels();

  // reset condition
  if (restart)
  {
    targetIndex = 0;
    previousMillis = 0;
    return false;
  }

  // finished if the target index is over the led limit
  if (targetIndex >= numberOfLedSegments)
    return true;

  // convert duration in delay for each segment
  const unsigned long delay = duration / (float)numberOfLedSegments;

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= delay and targetIndex < numberOfLedSegments) {
    previousMillis = currentMillis;

    strip.clear();

    // get and set corresponding hue color
    const uint16_t hue = map(targetIndex, 0, strip.numPixels(), 0, MAX_UINT16_T);
    strip.setPixelColor(targetIndex++, Adafruit_NeoPixel::gamma32(Adafruit_NeoPixel::ColorHSV(hue)));  //  Set pixel's color (in RAM)

    strip.show();  //  Update strip to match
  }

  return false;
}

bool colorWipeDown(const uint32_t color, const uint32_t duration, const bool restart, Adafruit_NeoPixel& strip) {
  static uint16_t targetIndex = 0;
  static unsigned long previousMillis = 0;
  static uint32_t nextColor = 0; 
  const uint16_t numberOfLedSegments = strip.numPixels();

  // reset condition
  if (restart)
  {
    targetIndex = 0;
    previousMillis = 0;
    nextColor = 0;
    return false;
  }

  // finished if the target index is over the led limit
  if (targetIndex >= numberOfLedSegments)
    return true;

  // convert duration in delay for each segment
  const unsigned long delay = duration / (float)numberOfLedSegments;

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= delay) {
    previousMillis = currentMillis;

    strip.setPixelColor(targetIndex++, color);
    nextColor = strip.getPixelColor(targetIndex);
  }
  else
  {
    // update the value of the last segement with a gradient
    const float coeff = (currentMillis - previousMillis) / (float)delay;
    strip.setPixelColor(targetIndex, get_gradient(nextColor, color, coeff));
  }

  strip.show();  //  Update strip to match
  return false;
}


bool colorWipeUp(const uint32_t color, const uint32_t duration, const bool restart, Adafruit_NeoPixel& strip) {
  const uint16_t numberOfLedSegments = strip.numPixels();
  static uint16_t targetIndex = numberOfLedSegments - 1;
  static unsigned long previousMillis = 0;
  static uint32_t nextColor = 0; 

  // reset condition
  if (restart)
  {
    targetIndex = numberOfLedSegments - 1;
    previousMillis = 0;
    nextColor = 0;
    return false;
  }

  // finished if the target index is over the led limit
  if (targetIndex >= numberOfLedSegments)
    return true;

  // convert duration in delay for each segment
  const unsigned long delay = duration / (float)numberOfLedSegments;

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= delay) {
    previousMillis = currentMillis;

    strip.setPixelColor(targetIndex--, color);
    nextColor = strip.getPixelColor(targetIndex);
  }
  else
  {
    // update the value of the last segement with a gradient
    const float coeff = (currentMillis - previousMillis) / (float)delay;
    strip.setPixelColor(targetIndex, get_gradient(nextColor, color, coeff));
  }

  strip.show();  //  Update strip to match
  return false;
}

};