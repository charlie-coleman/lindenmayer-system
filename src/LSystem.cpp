#include "LSystem.h"
#include "Util.h"

#include <iostream>
#include <algorithm>
#include <utility>
#include <random>
#include <chrono>

LSystem::LSystem()
  : m_generator(std::chrono::system_clock::now().time_since_epoch().count())
  , m_distribution(0.0f, 1.0f)
{
}

LSystem::~LSystem()
{
}

void LSystem::Configure(SystemConfigType& config)
{
  for (auto c : config.Constants)
  {
    if (c.second == "MOVE_FORWARD")
    {
      AddConstant(c.first, ActionEnum::MOVE_FORWARD);
    }
    else if (c.second == "DRAW_FORWARD")
    {
      AddConstant(c.first, ActionEnum::DRAW_FORWARD);
    }
    else if (c.second == "ROTATE_CW")
    {
      AddConstant(c.first, ActionEnum::ROTATE_CW);
    }
    else if (c.second == "ROTATE_CCW")
    {
      AddConstant(c.first, ActionEnum::ROTATE_CCW);
    }
    else if (c.second == "PUSH_STATE")
    {
      AddConstant(c.first, ActionEnum::PUSH_STATE);
    }
    else if (c.second == "POP_STATE")
    {
      AddConstant(c.first, ActionEnum::POP_STATE);
    }
    else
    {
      AddConstant(c.first, ActionEnum::NO_ACTION);
    }
  }

  SetAxiom(config.Axiom);

  for (auto constRules : config.Rules)
  {
    char constName = constRules.first;
    auto rules = constRules.second;
    
    auto constant = std::find_if(std::begin(m_constants), std::end(m_constants), [&](LConstant p) { return p.Name == constName;});
    if (constant == std::end(m_constants))
    {
      std::cerr << "Failed to match \"" << constName << "\" to an existing constant." << std::endl;
      continue;
    }

    for (auto weightedRule : rules)
    {
      float weight = weightedRule.first;
      std::string ruleStr = weightedRule.second;

      std::vector<LConstant> ruleVec;
      bool success = SplitStringIntoConstants(ruleStr, ruleVec);

      if (!success)
      {
        std::cerr << "Failed to split rule \"" << ruleStr << "\" string into constants." << std::endl;
        continue;
      }

      Rule rule(weight, ruleVec);
      m_constantRules[*constant].push_back(rule);
    }
  }
}

void LSystem::AddConstant(char value, ActionEnum action)
{
  m_constants.emplace_back(value, action);
}

bool LSystem::SetAxiom(std::string axiom)
{
  return SplitStringIntoConstants(axiom, m_axiom);
}

std::vector<LConstant> LSystem::GenerateNthAxiom(unsigned int n)
{
  if (n == 0)
  {
    return m_axiom;
  }
  
  std::vector<LConstant> output;
  auto precursor = GenerateNthAxiom(n - 1);

  for (const LConstant& c : precursor)
  {
    auto constantRules = m_constantRules.find(c);

    float r = m_distribution(m_generator);
    float totWeight = 0;
    
    if (constantRules != m_constantRules.end())
    {
      auto rules = constantRules->second;
      bool picked = false;
      for (int i = 0; i < rules.size(); ++i)
      {
        totWeight += rules[i].Weight;
        if (r < totWeight)
        {
          output.insert(output.end(), rules[i].Expansion.begin(), rules[i].Expansion.end());
          break;
        }
      }
    }
    else
    {
      std::cerr << "Failed to find \"" << c.Name << "\" in constants list." << std::endl; 
      break;
    }
  }

  return output;
}

bool LSystem::SetConstantRule(char constant, float weight, std::string rule)
{
  auto c = std::find_if(std::begin(m_constants), std::end(m_constants), [&](LConstant p) { return p.Name == constant;});

  if (c == std::end(m_constants))
  {
    std::cerr << "Failed to match \"" << constant << "\" to an existing constant." << std::endl;
    return false;
  }

  std::vector<LConstant> ruleVec;
  bool success = SplitStringIntoConstants(rule, ruleVec);

  if (success)
  {
    Rule rule(weight, ruleVec);
    m_constantRules[*c].push_back(rule);
  }

  return success;
}

void LSystem::Print()
{
  std::cout << "Constants: " << std::endl;
  for (auto& constant : m_constants)
  {
    auto rules = m_constantRules.find(constant);
    if (rules != m_constantRules.end())
    {
      for (auto& rule : rules->second)
      {
        std::cout << constant.Name << " -" << rule.Weight << "-> ";
        for (auto& c : rule.Expansion)
        {
          std::cout << c.Name;
        }
        std::cout << std::endl;
      }
    }
  }

  std::cout << "Axiom: ";
  for (auto& c : m_axiom)
  {
    std::cout << c.Name;
  }
  std::cout << std::endl;
}

bool LSystem::SplitStringIntoConstants(std::string input, std::vector<LConstant>& outputVec)
{
  for (char c : input)
  {
    auto constant = std::find_if(std::begin(m_constants), std::end(m_constants), [&](LConstant p) { return p.Name == c;});
    
    if (constant != std::end(m_constants))
    {
      outputVec.push_back(*constant);
    }
    else
    {
      std::cerr << "Failed to find \"" << c << "\" in constants." << std::endl;
      return false;
    }
  }

  return true;
}