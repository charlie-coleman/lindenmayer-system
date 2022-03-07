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
  };

  std::vector<std::string> split(std::string input, std::string delim);
  
  RGB HSV_To_RGB(HSV hsv);
}

#endif