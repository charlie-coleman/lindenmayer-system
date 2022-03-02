#include "Util.h"

std::vector<std::string> Util::split(std::string input, std::string delim)
{
  std::vector<std::string> output;

  if (!input.empty())
  {
    std::string::size_type start = 0;

    do
    {
      size_t x = input.find(delim, start);

      if (x == std::string::npos)
      {
        break;
      }

      output.push_back(input.substr(start, x-start));
      
      start += delim.size() + 1;
    } while (true);

    output.push_back(input.substr(start));
  }
  return output;
}