#ifndef _LSYSTEM_RENDERER_H_
#define _LSYSTEM_RENDERER_H_

#include "LSystem.h"
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
  LSystemRenderer(SDL_Window* window, std::vector<Constant>& axiom);
  LSystemRenderer(SDL_Window* window, std::vector<Constant>& axiom, float length, float lineWidth, float rotate, float startRotate);
  ~LSystemRenderer();

  void Center();
  void SetupGL(bool toScreen = true);
  void SetupRender();
  bool Render();
  bool RenderNextSteps(int steps = 1);

  void SetLength(float length);
  void SetLineWidth(float linewidth);
  void SetRotation(float rotate);
  void SetStartRotation(float startRotate);
  void SetCenter(bool center);

  void SetColorful(bool colorful);
  void SetSaturation(float saturation);
  void SetOrigin(float x, float y);

  void SetAxiom(std::vector<Constant>& axiom);

  bool SaveScreenshot(std::string filename, int padding = 20);

private:
  void RenderStep(int index);
  void DrawLine(float x1, float y1, float x2, float y2, float lineWidth);

  SDL_Window* m_window;

  float m_length;
  float m_lineWidth;
  float m_rotate;
  bool  m_center;
  bool  m_colorful;

  float m_startRot;
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

  std::vector<Constant>& m_axiom;

  Util::HSV m_color;

  std::stack<RendererState> m_stateStack;
};

#endif