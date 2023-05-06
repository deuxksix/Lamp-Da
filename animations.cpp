#include "animations.h"

#include "constants.h"
#include "wipes.h"

namespace animations
{

bool dotPingPong(const uint32_t color, const uint32_t duration, const bool restart, Adafruit_NeoPixel& strip)
{
  static bool isPongMode = false; // true: animation is climbing back the display

  // reset condition
  if (restart)
  {
    isPongMode = false;
    dotWipeUp(color, duration/2, restart, strip);
    dotWipeDown(color, duration/2, restart, strip);
    return false;
  }

  if (isPongMode)
  {
    return dotWipeUp(color, duration/2, false, strip);
  }
  else {
    // set pong mode to true when first animation is finished
    isPongMode = dotWipeDown(color, duration/2, false, strip);
  }

  // finished if the target index is over the led limit
  return false;
}


bool police(const uint32_t duration, const bool restart, Adafruit_NeoPixel& strip)
{
  static unsigned long previousMillis = 0;
  static bool isBluePhase = false;

  if (restart)
  {
    previousMillis = 0;
    isBluePhase = false;
    return false;
  }

  // convert duration in delay
  const uint16_t delay = duration / 2;

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= delay) {
    previousMillis = currentMillis;

    const uint16_t numberOfLedSegments = strip.numPixels();
  
    if (isBluePhase)
    {
      strip.clear();

      // blue lights
      strip.fill(strip.Color(0, 0, 255), 0, numberOfLedSegments / 2+1);  // Set blue
      strip.show();  // Update strip with new contents
      isBluePhase = false;
    }
    else {
      strip.clear();
      strip.fill(strip.Color(255, 0, 0), numberOfLedSegments / 2, numberOfLedSegments);  // Set red
      strip.show();  // Update strip with new contents

      isBluePhase = true;
    }

  }

  // never ends
  return false;
}

/**
 * \brief Compute the hue value of RGB to HSV
 */
uint16_t rgb2hue(const float r, const float g, const float b)
{
    const float cmax = max(r, max(g, b)); // maximum of r, g, b
    const float cmin = min(r, min(g, b)); // minimum of r, g, b
    const float diff = cmax - cmin; // diff of cmax and cmin.
  
    // if cmax and cmax are equal then h = 0
    if (cmax == cmin)
        return 0;
    // if cmax equal r then compute h
    else if (cmax == r)
        return fmod(60.0 * ((g - b) / diff) + 360, 360.0) / 360.0 * MAX_UINT16_T;
    // if cmax equal g then compute h
    else if (cmax == g)
        return fmod(60.0 * ((b - r) / diff) + 120, 360.0) / 360.0 * MAX_UINT16_T;
    // if cmax equal b then compute h
    else if (cmax == b)
        return fmod(60.0 * ((r - g) / diff) + 240, 360.0) / 360.0 * MAX_UINT16_T;

  return 0;
}


bool fadeOut(const uint32_t duration, const bool restart, Adafruit_NeoPixel& strip)
{
  static unsigned long startTimeMillis = 0;
  static uint8_t lastFadeLevel = 255;

  if (restart)
  {
    startTimeMillis = millis();
    lastFadeLevel = 255;
    return false;
  }
  if (lastFadeLevel == 0)
    return true;

  // get a fade level between 0 and 255
  const uint8_t fadeLevel = map(millis() - startTimeMillis, 0, duration, 255, 0);
  if (lastFadeLevel != fadeLevel)
  {
    lastFadeLevel = fadeLevel;

    // update all values of rgb
    const uint16_t numPixels = strip.numPixels();  
    for(uint16_t i = 0; i < numPixels; ++i)
    {
      const uint32_t pixelColor = strip.getPixelColor(i);
      if (pixelColor == 0)
        continue;
      
      // get RGB between 0 and 1
      const float red = ((pixelColor >> 16) & 255)/255.0;
      const float green = ((pixelColor >> 8) & 255)/255.0;
      const float blue = ((pixelColor >> 0) & 255)/255.0;

      const uint16_t hue = rgb2hue(red, green, blue);
      // diminish fade
      strip.setPixelColor(i, strip.ColorHSV(hue, 255, lastFadeLevel));
    }

    strip.show();
  }

  // fade in finished when we reach 255
  return lastFadeLevel == 0;
}


void rainbowFade2White(int wait, int rainbowLoops, Adafruit_NeoPixel& strip) {
  int fadeVal = 0, fadeMax = 100;

  // Hue of first pixel runs 'rainbowLoops' complete loops through the color
  // wheel. Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to rainbowLoops*65536, using steps of 256 so we
  // advance around the wheel at a decent clip.
  for (uint32_t firstPixelHue = 0; firstPixelHue < rainbowLoops * 65536;
       firstPixelHue += 256) {
    for (int i = 0; i < strip.numPixels(); i++) {  // For each pixel in strip...

      // Offset pixel hue by an amount to make one full revolution of the
      // color wheel (range of 65536) along the length of the strip
      // (strip.numPixels() steps):
      uint32_t pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());

      // strip.ColorHSV() can take 1 or 3 arguments: a hue (0 to 65535) or
      // optionally add saturation and value (brightness) (each 0 to 255).
      // Here we're using just the three-argument variant, though the
      // second value (saturation) is a constant 255.
      strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(
                                 pixelHue, 255, 255 * fadeVal / fadeMax)));
    }

    strip.show();
    delay(wait);

    if (firstPixelHue < 65536) {                                 // First loop,
      if (fadeVal < fadeMax) fadeVal++;                          // fade in
    } else if (firstPixelHue >= ((rainbowLoops - 1) * 65536)) {  // Last loop,
      if (fadeVal > 0) fadeVal--;                                // fade out
    } else {
      fadeVal = fadeMax;  // Interim loop, make sure fade is at max
    }
  }
}

}