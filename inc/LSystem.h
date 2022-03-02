#ifndef _LSYSTEM_H_
#define _LSYSTEM_H_

#include <string>
#include <vector>
#include <queue>
#include <map>

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

typedef std::pair<char, ActionEnum> Constant;

class LSystem
{
public:
  LSystem();
  ~LSystem();

  void AddConstant(char value, ActionEnum action);
  
  bool SetAxiom(std::string axiom);
  bool SetConstantRule(char value, std::string rule);

  std::vector<Constant> GenerateNthAxiom(unsigned int n);

  void Print();

private:
  bool SplitStringIntoConstants(std::string input, std::vector<Constant>& outputVec);

  std::vector<Constant> m_axiom;
  std::vector<Constant> m_constants;
  std::map<Constant, std::vector<Constant>> m_constantRules;
};

#endif