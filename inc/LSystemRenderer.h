#ifndef _LSYSTEM_RENDERER_H_
#define _LSYSTEM_RENDERER_H_

#include "LSystem.h"
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

class LSystemRenderer
{
public:
  LSystemRenderer(SDL_Window* window);
  LSystemRenderer(SDL_Window* window, float length, float lineWidth, float rotate, float startRotate);
  ~LSystemRenderer();

  bool Render(std::vector<Constant> axiom, bool toScreen = true);

  void SetLength(float length);
  void SetLineWidth(float linewidth);
  void SetRotation(float rotate);
  void SetStartRotation(float startRotate);
  void SetCenter(bool center);

  void SetColorful(bool colorful);
  void SetSaturation(float saturation);
  void SetOrigin(float x, float y);

  bool SaveScreenshot(std::string filename, int padding = 20);

private:
  void DrawLine(float x1, float y1, float x2, float y2, float lineWidth);

  bool CenterOrigin(float minX, float maxX, float minY, float maxY);
  RGB HSV_To_RGB(HSV hsv);

  SDL_Window* m_window;

  bool m_center;
  bool m_drawn;

  bool m_colorful;
  float m_saturation;

  float m_length;
  float m_lineWidth;
  float m_rotate;

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

  HSV m_color;

  std::stack<RendererState> m_stateStack;
};

#endif