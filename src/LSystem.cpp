#include "LSystem.h"
#include "Util.h"

#include <iostream>
#include <algorithm>
#include <utility>

LSystem::LSystem()
{
}

LSystem::~LSystem()
{
}

void LSystem::AddConstant(char value, ActionEnum action)
{
  Constant c = std::make_pair(value, action);
  m_constants.push_back(c);
}

bool LSystem::SetAxiom(std::string axiom)
{
  return SplitStringIntoConstants(axiom, m_axiom);
}

std::vector<Constant> LSystem::GenerateNthAxiom(unsigned int n)
{
  if (n == 1)
  {
    return m_axiom;
  }
  
  std::vector<Constant> output, precursor;
  precursor = GenerateNthAxiom(n - 1);

  for (auto iter = precursor.begin(); iter != precursor.end(); ++iter)
  {
    auto rule = m_constantRules.find(*iter);
    if (rule != m_constantRules.end())
    {
      output.insert(output.end(), rule->second.begin(), rule->second.end());
    }
    else
    {
      std::cerr << "Failed to find \"" << iter->first << "\" in constants list." << std::endl; 
    }
  }

  return output;
}

bool LSystem::SetConstantRule(char constant, std::string rule)
{
  auto c = std::find_if(std::begin(m_constants), std::end(m_constants), [&](Constant p) { return p.first == constant;});

  if (c == std::end(m_constants))
  {
    std::cerr << "Failed to match \"" << constant << "\" to an existing constant." << std::endl;
    return false;
  }

  std::vector<Constant> ruleVec;
  bool success = SplitStringIntoConstants(rule, ruleVec);

  if (success)
  {
    m_constantRules[*c] = ruleVec;
  }

  return success;
}

void LSystem::Print()
{
  std::cout << "Constants: " << std::endl;
  for (auto iter = m_constants.begin(); iter != m_constants.end(); ++iter)
  {
    std::cout << iter->first;

    if (m_constantRules.find(*iter) != m_constantRules.end())
    {
      auto rules = m_constantRules[*iter];
      std::cout << " -> ";
      for (auto ruleIter = rules.begin(); ruleIter != rules.end(); ++ruleIter)
      {
        std::cout << ruleIter->first;
      }
    }

    std::cout << std::endl;
  }

  std::cout << "Axiom: ";
  for (auto iter = m_axiom.begin(); iter != m_axiom.end(); ++iter)
  {
    std::cout << iter->first;
  }
  std::cout << std::endl;
}

bool LSystem::SplitStringIntoConstants(std::string input, std::vector<Constant>& outputVec)
{
  for (auto iter = input.begin(); iter != input.end(); ++iter)
  {
    auto c = std::find_if(std::begin(m_constants), std::end(m_constants), [&](Constant p) { return p.first == *iter;});
    
    if (c != std::end(m_constants))
    {
      outputVec.push_back(*c);
    }
    else
    {
      std::cerr << "Failed to find \"" << *iter << "\" in constants." << std::endl;
      return false;
    }
  }

  return true;
}