#include "Util.h"
#include <cmath>

std::vector<std::string> Util::split(std::string input, const char delim)
{
  std::vector<std::string> output;

  size_t start;
  size_t end = 0;

  while ((start = input.find_first_not_of(delim, end)) != std::string::npos)
  {
    end = input.find(delim, start);
    output.push_back(input.substr(start, end - start));
  }

  return output;
}

Util::RGB Util::HSV_To_RGB(Util::HSV hsv)
{
  int i = (int)floorf(hsv.Hue * 6);
  float f = hsv.Hue * 6.0f - i;
  float p = hsv.Value * (1.0f - hsv.Saturation);
  float q = hsv.Value * (1.0f - f * hsv.Saturation);
  float t = hsv.Value * (1.0f - (1.0f - f) * hsv.Saturation);

  Util::RGB output;
  switch (i % 6)
  {
    case 0:
      output.Red = hsv.Value;
      output.Green = t;
      output.Blue = p;
      break;
    case 1:
      output.Red = q;
      output.Green = hsv.Value;
      output.Blue = p;
      break;
    case 2:
      output.Red = p;
      output.Green = hsv.Value;
      output.Blue = t;
      break;
    case 3:
      output.Red = p;
      output.Green = q;
      output.Blue = hsv.Value;
      break;
    case 4:
      output.Red = t;
      output.Green = p;
      output.Blue = hsv.Value;
      break;
    case 5:
    default:
      output.Red = hsv.Value;
      output.Green = p;
      output.Blue = q;
      break;
  }

  return output;
}