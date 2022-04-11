#ifndef _CONFIG_PARSER_H_
#define _CONFIG_PARSER_H_

#include "INIReader.h"
#include "Util.h"

#include <string>
#include <vector>

struct WindowConfigType
{
  bool Display;
  int Width;
  int Height;
  int Framerate;
};

struct GeneralConfigType
{
  int Generation;
  int Length;
  int LineWidth;
  bool Animate;
  float AnimateTime;
  float EndFrameTime;
  float Angle;
  float StartingRotation;
  bool Center;
  int FixedX;
  int FixedY;
  bool Colorful;
  Util::RGB Color;
  Util::RGB Background;
  float Saturation;
  int Padding;
};

struct SystemConfigType
{
  std::vector<std::pair<char, std::string>> Constants;
  std::string Axiom;
  std::map<char, std::vector<std::pair<float, std::string>>> Rules;
};

struct ConfigurationType
{
  WindowConfigType Window;
  GeneralConfigType General;
  SystemConfigType System;
};

class ConfigParser
{
public:
  ConfigParser(std::string filename, ConfigurationType& config);
  ~ConfigParser() {}

protected:
  void ParseWindowConfiguration(INIReader& ini, ConfigurationType& config);
  void ParseGeneralConfiguration(INIReader& ini, ConfigurationType& config);
  void ParseSystemConfiguration(INIReader& ini, ConfigurationType& config);
  void ParseRuleConfiguration(INIReader& ini, ConfigurationType& config);

  Util::RGB ParseColorString(std::string color);
};

#endif