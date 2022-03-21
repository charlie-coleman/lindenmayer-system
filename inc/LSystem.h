#ifndef _LSYSTEM_H_
#define _LSYSTEM_H_

#include <string>
#include <vector>
#include <unordered_map>
#include <random>

#include "ConfigParser.h"

enum class ActionEnum
{
  NO_ACTION = 0,
  MOVE_FORWARD,
  DRAW_FORWARD,
  ROTATE_CW,
  ROTATE_CCW,
  PUSH_STATE,
  POP_STATE,
  SIZE_OF_CONSTANT_ACTION
};

struct LConstant
{
  char Name;
  ActionEnum Action;

  LConstant(char name, ActionEnum act)
  {
    Name = name;
    Action = act;
  }

  bool operator==(const LConstant& c) const
  {
    return (c.Name == Name) && (c.Action == Action);
  }
};

namespace std
{
  template <>
  struct hash<LConstant>
  {
    std::size_t operator()(const LConstant& c) const
    {
      using std::hash;

      return (hash<char>()(c.Name) ^ (hash<int>()((int)c.Action) << 1));
    }
  };
};

struct Rule
{
  float Weight;
  std::vector<LConstant> Expansion;

  Rule(float weight, std::vector<LConstant> exp)
  {
    Weight = weight;
    Expansion = exp;
  }
};

class LSystem
{
public:
  LSystem();
  ~LSystem();

  void Configure(SystemConfigType& config);

  void AddConstant(char value, ActionEnum action);
  bool SetAxiom(std::string axiom);
  bool SetConstantRule(char value, float weight, std::string rule);

  std::vector<LConstant> GenerateNthAxiom(unsigned int n);

  void Print();

private:
  bool SplitStringIntoConstants(std::string input, std::vector<LConstant>& outputVec);

  std::mt19937 m_generator;
  std::uniform_real_distribution<float> m_distribution;

  std::vector<LConstant> m_axiom;
  std::vector<LConstant> m_constants;
  std::unordered_map<LConstant, std::vector<Rule>> m_constantRules;
};

#endif