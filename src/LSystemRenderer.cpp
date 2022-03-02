#include "LSystemRenderer.h"

#include "glew.h"
#include "GL/GL.h"
#include "SDL_opengl.h"
#include <cmath>
#include <iostream>
#include <limits>

LSystemRenderer::LSystemRenderer(SDL_Window* window)
  : m_window(window)
  , m_length(1)
  , m_lineWidth(1.0f)
  , m_rotate(90.0f)
  , m_origX(0.0f)
  , m_origY(0.0f)
  , m_startRot(0.0f)
  , m_center(true)
  , m_drawn(false)
  , m_colorful(false)
  , m_saturation(0.5f)
  , m_stateStack()
{
  m_color.Hue = 0;
  m_color.Saturation = m_saturation;
  m_color.Value = 1;
}

LSystemRenderer::LSystemRenderer(SDL_Window* window, float length, float lineWidth, float rotate, float startRotate)
  : m_window(window)
  , m_length(length)
  , m_lineWidth(lineWidth)
  , m_rotate(rotate)
  , m_startRot(startRotate)
  , m_origX(0.0f)
  , m_origY(0.0f)
  , m_center(true)
  , m_drawn(false)
  , m_colorful(false)
  , m_saturation(0.5f)
  , m_stateStack()
{
  m_color.Hue = 0;
  m_color.Saturation = m_saturation;
  m_color.Value = 1;
}

LSystemRenderer::~LSystemRenderer()
{
}

void LSystemRenderer::SetLength(float length)
{
  m_length = length;
}

void LSystemRenderer::SetLineWidth(float lineWidth)
{
  m_lineWidth = lineWidth;
}

void LSystemRenderer::SetRotation(float rotate)
{
  m_rotate = rotate;
}

void LSystemRenderer::SetStartRotation(float startRotate)
{
  m_startRot = startRotate;
}

void LSystemRenderer::SetOrigin(float x, float y)
{
  m_origX = x;
  m_origY = y;
}

void LSystemRenderer::SetCenter(bool center)
{
  m_center = center;
}

void LSystemRenderer::SetColorful(bool colorful)
{
  m_colorful = colorful;
}

void LSystemRenderer::SetSaturation(float saturation)
{
  m_saturation = saturation;
  m_color.Saturation = saturation;
}

RGB LSystemRenderer::HSV_To_RGB(HSV hsv)
{
  int i = (int)floorf(hsv.Hue * 6);
  float f = hsv.Hue * 6.0f - i;
  float p = hsv.Value * (1.0f - hsv.Saturation);
  float q = hsv.Value * (1.0f - f * hsv.Saturation);
  float t = hsv.Value * (1.0f - (1.0f - f) * hsv.Saturation);

  RGB output;
  switch (i % 6)
  {
    case 0:
      output.Red = hsv.Value;
      output.Green = t;
      output.Blue = p;
      break;
    case 1:
      output.Red = q;
      output.Green = hsv.Value;
      output.Blue = p;
      break;
    case 2:
      output.Red = p;
      output.Green = hsv.Value;
      output.Blue = t;
      break;
    case 3:
      output.Red = p;
      output.Green = q;
      output.Blue = hsv.Value;
      break;
    case 4:
      output.Red = t;
      output.Green = p;
      output.Blue = hsv.Value;
      break;
    case 5:
    default:
      output.Red = hsv.Value;
      output.Green = p;
      output.Blue = q;
      break;
  }

  return output;
}

bool LSystemRenderer::CenterOrigin(float minX, float maxX, float minY, float maxY)
{
    float centerX = (maxX + minX) / 2.0f;
    float centerY = (maxY + minY) / 2.0f;

    int width, height;
    SDL_GetWindowSize(m_window, &width, &height);

    float diffX = centerX - (static_cast<float>(width) / 2.0f);
    float diffY = centerY - (static_cast<float>(height) / 2.0f);

    m_origX -= (int)diffX;
    m_origY -= (int)diffY;

    return (std::abs(diffX) < 5.0f && std::abs(diffY) < 5.0f);
}

bool LSystemRenderer::SaveScreenshot(std::string filepath, int padding)
{
  int windowWidth, windowHeight;
  SDL_GetWindowSize(m_window, &windowWidth, &windowHeight);

  unsigned int width = std::abs(m_maxX - m_minX) + (2 * padding);
  unsigned int height = std::abs(m_maxY - m_minY) + (2 * padding);

  unsigned int x, y, w, h;
  if (width > windowWidth)
  {
    x = 0;
    w = windowWidth;
  }
  else
  {
    x = m_minX - padding;
    w = width;
  }
  
  if (height > windowHeight)
  {
    y = 0;
    h = windowHeight;
  }
  else
  {
    y = m_minY - padding;
    h = height;
  }

  size_t arraySize = w * h;
  unsigned int* pixels = new unsigned int[arraySize];

  glReadPixels(x, y, w, h, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, pixels);
  GLenum error = glGetError();
  if (error != GL_NO_ERROR)
  {
    std::cerr << "glReadPixels error: " << error << std::endl;
    return false;
  }

  SDL_Surface* sshot = SDL_CreateRGBSurfaceFrom(pixels, w, h, 8*4, w*4, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
  if (sshot == NULL)
  {
    std::cerr << "Creating surface failed: " << SDL_GetError() << std::endl;
    return false;
  }
  
  int err = SDL_SaveBMP(sshot, filepath.c_str());
  if (err != 0)
  {
    std::cerr << "Saving BMP failed: " << SDL_GetError() << std::endl;
    return false;
  }

  SDL_FreeSurface(sshot);
  delete[] pixels;

  return true;
}

void LSystemRenderer::DrawLine(float x1, float y1, float x2, float y2, float lineWidth)
{
  GLfloat lineWidthRange[2] = {0.0f, 0.0f};
  glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, lineWidthRange);

  if (lineWidth > lineWidthRange[1])
  {
    std::cerr << "Line width supplied (" << lineWidth << ") is outside supported range. Changing to " << lineWidthRange[1] << std::endl;
    lineWidth = lineWidthRange[1];
  }
  else if (lineWidth < lineWidthRange[0])
  {
    std::cerr << "Line width supplied (" << lineWidth << ") is outside supported range. Changing to " << lineWidthRange[0] << std::endl;
    lineWidth = lineWidthRange[0];
  }

  glLineWidth(lineWidth);
  glPointSize(lineWidth);

  glBegin(GL_LINES);
  glVertex2f(x1, y1);
  glVertex2f(x2, y2);
  glEnd();

  glBegin(GL_POINTS);
  glVertex2f(x1, y1);
  glVertex2f(x2, y2);
  glEnd();
}

bool LSystemRenderer::Render(std::vector<Constant> axiom, bool toScreen)
{
  int width, height;
  SDL_GetWindowSize(m_window, &width, &height);
  glViewport(0, 0, width, height);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, width, 0, height, -1, 1);

  if (toScreen)
    glTranslatef(0.5f, 0.5f, 0.0f);
    
  glMatrixMode(GL_MODELVIEW);

  glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
  m_stateStack = std::stack<RendererState>();

  m_x = m_origX;
  m_y = m_origY;
  m_currRot = m_startRot;

  m_minX = std::numeric_limits<float>::max();
  m_maxX = std::numeric_limits<float>::min();

  m_minY = std::numeric_limits<float>::max();
  m_maxY = std::numeric_limits<float>::min();

  int color_i = 0;
  for (auto iter = axiom.begin(); iter != axiom.end(); ++iter)
  {
    if (m_colorful)
    {
      m_color.Hue = (float)color_i / (float)axiom.size();
      
      RGB rgb = HSV_To_RGB(m_color);

      glColor4f(rgb.Red, rgb.Green, rgb.Blue, 1.0f);
    }

    float new_x = m_x + m_length * cosf(m_currRot * PI / 180.0f);
    float new_y = m_y + m_length * sinf(m_currRot * PI / 180.0f);

    RendererState s;
    switch (iter->second)
    {
      case ActionEnum::DRAW_FORWARD:
        DrawLine(m_x, m_y, new_x, new_y, m_lineWidth);
        m_x = new_x;
        m_y = new_y;
        break;
      case ActionEnum::MOVE_FORWARD:
        m_x = new_x;
        m_y = new_y;
        break;
      case ActionEnum::ROTATE_CW:
        m_currRot += m_rotate;
        m_currRot = fmodf(m_currRot, 360.0f);
        break;
      case ActionEnum::ROTATE_CCW:
        m_currRot -= m_rotate;
        m_currRot = fmodf(m_currRot, 360.0f);
        break;
      case ActionEnum::PUSH_STATE:
        s.X = m_x;
        s.Y = m_y;
        s.Rotation = m_currRot;
        m_stateStack.push(s);
        break;
      case ActionEnum::POP_STATE:
        s = m_stateStack.top();
        m_stateStack.pop();
        m_x = s.X;
        m_y = s.Y;
        m_currRot = s.Rotation;
        break;
      case ActionEnum::NO_ACTION:
      default:
        break;
    }

    ++color_i;
    if (m_x > m_maxX) m_maxX = m_x;
    if (m_x < m_minX) m_minX = m_x;
    if (m_y > m_maxY) m_maxY = m_y;
    if (m_y < m_minY) m_minY = m_y;
  }

  m_drawn = true;

  if (m_center)
  {
    return CenterOrigin(m_minX, m_maxX, m_minY, m_maxY);
  }

  return true;
}