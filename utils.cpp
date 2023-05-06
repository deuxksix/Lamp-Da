#include "utils.h"

#include <Adafruit_NeoPixel.h>

#include <stdlib.h>
#include <math.h>

uint32_t get_random_color()
{
    return (rand()%255) << 16 | (rand()%255) << 8 | (rand()%255);
}

uint32_t get_complementary_color(const uint32_t color)
{
    const float red = ((color >> 16) & 255)/255.0;
    const float green = ((color >> 8) & 255)/255.0;
    const float blue = ((color >> 0) & 255)/255.0;

    const uint32_t hue = rgb2hue(red, green, blue);

    // add a cardan shift to the hue, to opbtain the symetrical color
    return Adafruit_NeoPixel::ColorHSV(hue + MAX_UINT16_T/2, 255, 255);
}

uint32_t get_random_complementary_color(const uint32_t color, const float tolerance)
{
    const float red = ((color >> 16) & 255)/255.0;
    const float green = ((color >> 8) & 255)/255.0;
    const float blue = ((color >> 0) & 255)/255.0;

    const uint32_t hue = rgb2hue(red, green, blue);

    // add random offset
    return Adafruit_NeoPixel::ColorHSV(hue + MAX_UINT16_T/2 + (rand()%MAX_UINT16_T) * tolerance, 255, 255);
}

uint16_t rgb2hue(const float r, const float g, const float b)
{
    const float cmax = fmax(r, fmax(g, b)); // maximum of r, g, b
    const float cmin = fmin(r, fmin(g, b)); // minimum of r, g, b
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