#ifndef _UTIL_H_
#define _UTIL_H_

#include <vector>
#include <string>

namespace Util
{
  struct HSV
  {
    float Hue;
    float Saturation;
    float Value;
  };

  struct RGB
  {
    float Red;
    float Green;
    float Blue;

    RGB(float r, float g, float b)
      : Red(r)
      , Green(g)
      , Blue(b)
    {
    }

    RGB(const RGB& rgb) : RGB(rgb.Red, rgb.Green, rgb.Blue)
    {
    }

    RGB() : RGB(0.0,0.0,0.0)
    {
    }
  };

  std::vector<std::string> split(std::string input, const char delim);
  
  RGB HSV_To_RGB(HSV hsv);
}

#endif