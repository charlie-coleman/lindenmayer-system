#include "ConfigParser.h"
#include "Util.h"
#include <iostream>

ConfigParser::ConfigParser(std::string filename, ConfigurationType& config)
{
  INIReader ini(filename);
  ParseWindowConfiguration(ini, config);
  ParseGeneralConfiguration(ini, config);
  ParseSystemConfiguration(ini, config);
  ParseRuleConfiguration(ini, config);
}

void ConfigParser::ParseWindowConfiguration(INIReader& ini, ConfigurationType& config)
{
  config.Window.Width     = ini.GetInteger("window", "width", 1600);
  config.Window.Height    = ini.GetInteger("window", "height", 900);
  config.Window.Framerate = ini.GetInteger("window", "framerate", 60);
  config.Window.Display   = ini.GetBoolean("window", "display", true);
}

void ConfigParser::ParseGeneralConfiguration(INIReader& ini, ConfigurationType& config)
{
  config.General.Generation       = ini.GetInteger("general", "generation", 3);
  config.General.Length           = ini.GetInteger("general", "length", 5);
  config.General.Angle            = ini.GetFloat("general", "angle", 90.0f);
  config.General.Animate          = ini.GetBoolean("general", "animate", false);
  config.General.AnimateTime      = ini.GetFloat("general", "animatetime", 12.5);
  config.General.EndFrameTime     = ini.GetFloat("general", "endframetime", 2.5);
  config.General.StartingRotation = ini.GetFloat("general", "startingrotation", 0.0f);
  config.General.LineWidth        = ini.GetFloat("general", "linewidth", 1.0f);
  config.General.Center           = ini.GetBoolean("general", "center", true);
  config.General.Colorful         = ini.GetBoolean("general", "colorful", false);
  config.General.Saturation       = ini.GetFloat("general", "saturation", 0.6f);
  config.General.Padding          = ini.GetInteger("general", "padding", 20);
}

void ConfigParser::ParseSystemConfiguration(INIReader& ini, ConfigurationType& config)
{
  std::string constants = ini.Get("lsystem", "constants", "");
  for (int i = 0; i < constants.size(); ++i)
  {
    std::string constantName = "constant" + std::to_string(i);
    std::string action = ini.Get(constantName, "action", "NO_ACTION");

    config.System.Constants.push_back(std::make_pair(constants[i], action));
  }

  config.System.Axiom = ini.Get("lsystem", "axiom", "");
}

void ConfigParser::ParseRuleConfiguration(INIReader& ini, ConfigurationType& config)
{
  for (int i = 0; i < config.System.Constants.size(); ++i)
  {
    std::string constantName = "constant" + std::to_string(i);
    std::string weightString = ini.Get(constantName, "weights", "1.0");

    float totalWeight = 0;
    std::vector<float> weights;
    std::vector<std::string> weightStrings = Util::split(weightString, ',');
    for (auto w : weightStrings)
    {
      float weight = std::stof(w);
      totalWeight += weight;
      weights.push_back(weight);
    }

    if (totalWeight != 1.0f)
    {
      std::cout << "Given weights don't sum to 1.0. May cause issues/unexpected behavior with a total weight of " << totalWeight << std::endl;
    }

    for (int j = 0; j < weights.size(); ++j)
    {
      std::string ruleName = "rule" + std::to_string(j);
      std::string rule = ini.Get(constantName, ruleName, std::string(1, config.System.Constants[i].first));

      config.System.Rules[config.System.Constants[i].first].push_back(std::make_pair(weights[j], rule));
    }
  }
}