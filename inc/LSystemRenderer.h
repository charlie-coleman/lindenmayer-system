#ifndef _LSYSTEM_RENDERER_H_
#define _LSYSTEM_RENDERER_H_

#include "LSystem.h"
#include "ConfigParser.h"
#include "Util.h"

#include "SDL.h"
#include <memory>
#include <stack>

#define PI 3.14159265358979323846

struct RendererState
{
  int   X;
  int   Y;
  float Rotation;
};

class LSystemRenderer
{
public:
  LSystemRenderer(SDL_Window* window, std::vector<LConstant>& axiom, const ConfigurationType& config);
  ~LSystemRenderer();

  void Center();
  void SetupGL(bool toScreen = true);
  void SetupRender();
  bool Render();
  bool RenderNextSteps(int steps = 1);

  void SetOrigin(float x, float y);

  void SetAxiom(std::vector<LConstant>& axiom);

  bool SaveScreenshot(const std::string& filename, int padding = 20);

private:
  void RenderStep(int index);
  void DrawLine(float x1, float y1, float x2, float y2);

  SDL_Window* m_window;

  const ConfigurationType& m_config;

  float m_lineWidth;

  float m_origX;
  float m_origY;
  float m_minX;
  float m_minY;
  float m_maxX;
  float m_maxY;
  float m_currRot;
  float m_x;
  float m_y;

  int m_windowWidth;
  int m_windowHeight;

  int m_drawIndex;

  std::vector<LConstant>& m_axiom;

  Util::HSV m_color;

  std::stack<RendererState> m_stateStack;
};

#endif