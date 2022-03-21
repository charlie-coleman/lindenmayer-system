#include "LSystemRenderer.h"

#include "glew.h"
#include "GL/GL.h"
#include "SDL_opengl.h"
#include "savepng.h"
#include <cmath>
#include <iostream>
#include <limits>

LSystemRenderer::LSystemRenderer(SDL_Window* window, std::vector<LConstant>& axiom)
  : m_window(window)
  , m_axiom(axiom)
  , m_length(1)
  , m_lineWidth(1.0f)
  , m_rotate(90.0f)
  , m_origX(0.0f)
  , m_origY(0.0f)
  , m_startRot(0.0f)
  , m_center(true)
  , m_colorful(false)
  , m_stateStack()
  , m_drawIndex(0)
{
  m_color.Hue = 0;
  m_color.Saturation = 0.5f;
  m_color.Value = 1;
  
  SDL_GetWindowSize(m_window, &m_windowWidth, &m_windowHeight);
}

LSystemRenderer::LSystemRenderer(SDL_Window* window, std::vector<LConstant>& axiom, float length, float lineWidth, float rotate, float startRotate)
  : m_window(window)
  , m_axiom(axiom)
  , m_length(length)
  , m_lineWidth(lineWidth)
  , m_rotate(rotate)
  , m_startRot(startRotate)
  , m_origX(0.0f)
  , m_origY(0.0f)
  , m_center(true)
  , m_colorful(false)
  , m_stateStack()
  , m_drawIndex(0)
{
  m_color.Hue = 0;
  m_color.Saturation = 0.5f;
  m_color.Value = 1;

  SDL_GetWindowSize(m_window, &m_windowWidth, &m_windowHeight);
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
  m_color.Saturation = saturation;
}

void LSystemRenderer::SetAxiom(std::vector<LConstant>& axiom)
{
  m_axiom = axiom;
}

void LSystemRenderer::Center()
{
  m_x = m_origX;
  m_y = m_origY;
  m_currRot = m_startRot;
  m_stateStack = std::stack<RendererState>();

  m_minX = std::numeric_limits<float>::max();
  m_maxX = std::numeric_limits<float>::min();

  m_minY = std::numeric_limits<float>::max();
  m_maxY = std::numeric_limits<float>::min();

  for (auto iter = m_axiom.begin(); iter != m_axiom.end(); ++iter)
  {
    float new_x = m_x + m_length * cosf(m_currRot * PI / 180.0f);
    float new_y = m_y + m_length * sinf(m_currRot * PI / 180.0f);

    RendererState s;
    switch (iter->Action)
    {
      case ActionEnum::DRAW_FORWARD:
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

    if (m_x > m_maxX) m_maxX = m_x;
    if (m_x < m_minX) m_minX = m_x;
    if (m_y > m_maxY) m_maxY = m_y;
    if (m_y < m_minY) m_minY = m_y;
  }

  float centerX = (m_maxX + m_minX) / 2.0f;
  float centerY = (m_maxY + m_minY) / 2.0f;

  float diffX = centerX - (static_cast<float>(m_windowWidth) / 2.0f);
  float diffY = centerY - (static_cast<float>(m_windowHeight) / 2.0f);

  int width = std::abs(m_maxX - m_minX);
  int height = std::abs(m_maxY - m_minY);
  std::cout << "Resultant curve is " << width << " pixels by " << height << " pixels." << std::endl;
  if (width > m_windowWidth || height > m_windowHeight)
  {
    std::cout << "Warning: Resultant curve is larger than current window size and will be partially obscured." << std::endl;
  }

  std::cout << "Adjusting origin by (" << diffX << ", " << diffY << ")." << std::endl; 

  m_minX -= (int)diffX;
  m_minY -= (int)diffY;
  m_maxX -= (int)diffX;
  m_maxY -= (int)diffY;
  m_origX -= (int)diffX;
  m_origY -= (int)diffY;
}

bool LSystemRenderer::SaveScreenshot(const std::string& filepath, int padding)
{
  unsigned int width = std::abs(m_maxX - m_minX) + (2 * padding);
  unsigned int height = std::abs(m_maxY - m_minY) + (2 * padding);

  unsigned int x, y, w, h;
  if (width > m_windowWidth)
  {
    x = 0;
    w = m_windowWidth;
  }
  else
  {
    x = m_minX - padding;
    w = width;
  }
  
  if (height > m_windowHeight)
  {
    y = 0;
    h = m_windowHeight;
  }
  else
  {
    y = m_minY - padding;
    h = height;
  }

  size_t arraySize = w * h;
  unsigned int* pixels = new unsigned int[arraySize];

  glReadPixels(x, y, w, h, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, pixels);
  GLenum error = glGetError();
  if (error != GL_NO_ERROR)
  {
    std::cerr << "glReadPixels error: " << error << std::endl;
    return false;
  }

  SDL_Surface* sshot = SDL_CreateRGBSurfaceFrom(pixels, w, h, 8*4, w*4, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);
  if (sshot == NULL)
  {
    std::cerr << "Creating surface failed: " << SDL_GetError() << std::endl;
    return false;
  }

  if (sshot->h > 2)
  {
    Uint8 *t;
    Uint8 *a, *b;
    Uint8 *last;
    Uint16 pitch;

    /* get a place to store a line */
    pitch = sshot->pitch;
    t = (Uint8*)malloc(pitch);

    if (t == NULL) {
        std::cerr << "Failed while flipping surface." << std::endl;
        return false;
    }

    /* get first line; it's about to be trampled */
    memcpy(t, sshot->pixels,pitch);

    /* now, shuffle the rest so it's almost correct */
    a = (Uint8*)sshot->pixels;
    last = a + pitch * (sshot->h - 1);
    b = last;

    while(a < b) {
        memcpy(a,b,pitch);
        a += pitch;
        memcpy(b,a,pitch);
        b -= pitch;
    }

    /* in this shuffled state, the bottom slice is too far down */
    memmove( b, b+pitch, last-b );
    /* now we can put back that first row--in the last place */
    memcpy(last,t,pitch);
    /* everything is in the right place; close up. */
    free(t);
  }

  int err = SDL_SavePNG(sshot, filepath.c_str());
  if (err != 0)
  {
    std::cerr << "Saving PNG failed: " << SDL_GetError() << std::endl;
    return false;
  }

  SDL_FreeSurface(sshot);
  delete[] pixels;

  return true;
}

void LSystemRenderer::DrawLine(float x1, float y1, float x2, float y2)
{
  GLfloat lineWidthRange[2] = {0.0f, 0.0f};
  glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, lineWidthRange);

  if (m_lineWidth > lineWidthRange[1])
  {
    std::cerr << "Line width supplied (" << m_lineWidth << ") is outside supported range. Changing to " << lineWidthRange[1] << std::endl;
    m_lineWidth = lineWidthRange[1];
  }
  else if (m_lineWidth < lineWidthRange[0])
  {
    std::cerr << "Line width supplied (" << m_lineWidth << ") is outside supported range. Changing to " << lineWidthRange[0] << std::endl;
    m_lineWidth = lineWidthRange[0];
  }

  glLineWidth(m_lineWidth);
  glPointSize(m_lineWidth);

  glBegin(GL_LINES);
  glVertex2i(x1, y1);
  glVertex2i(x2, y2);
  glEnd();

  glBegin(GL_POINTS);
  glVertex2i(x1, y1);
  glVertex2i(x2, y2);
  glEnd();
}

void LSystemRenderer::SetupGL(bool toScreen)
{
  glViewport(0, 0, m_windowWidth, m_windowHeight);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, m_windowWidth, 0, m_windowHeight, -1, 1);

  glTranslatef(0.375f, 0.375f, 0.0f);
    
  glMatrixMode(GL_MODELVIEW);

  glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
      
  glClear(GL_COLOR_BUFFER_BIT);
}

void LSystemRenderer::SetupRender()
{
  m_x = m_origX;
  m_y = m_origY;
  m_currRot = m_startRot;
  m_stateStack = std::stack<RendererState>();
}

void LSystemRenderer::RenderStep(int index)
{
  if (m_colorful)
  {
    m_color.Hue = (float)index / (float)m_axiom.size();
    
    Util::RGB rgb = Util::HSV_To_RGB(m_color);

    glColor4f(rgb.Red, rgb.Green, rgb.Blue, 1.0f);
  }

  float new_x = m_x + m_length * cosf(m_currRot * PI / 180.0f);
  float new_y = m_y + m_length * sinf(m_currRot * PI / 180.0f);

  LConstant c = m_axiom[index];

  RendererState s;
  switch (c.Action)
  {
    case ActionEnum::DRAW_FORWARD:
      DrawLine(m_x, m_y, new_x, new_y);
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
}

bool LSystemRenderer::RenderNextSteps(int steps)
{
  int endIndex = m_drawIndex + steps;
  for (m_drawIndex = 0; m_drawIndex < std::min((int)m_axiom.size(), endIndex); ++m_drawIndex)
  {
    RenderStep(m_drawIndex);
  }

  return (m_drawIndex >= m_axiom.size());
}

bool LSystemRenderer::Render()
{
  for (m_drawIndex = 0; m_drawIndex < m_axiom.size(); ++m_drawIndex)
  {
    RenderStep(m_drawIndex);
  }

  return true;
}