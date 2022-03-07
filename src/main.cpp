#include <iostream>
#include <cmath>
#include <filesystem>

#define SDL_MAIN_HANDLED

#include "glew.h"
#include "GL/GL.h"
#include "SDL_opengl.h"
#include "SDL.h"
#include "INIReader.h"
#include "LSystem.h"
#include "LSystemRenderer.h"

struct WindowConfigType
{
  bool Display;
  int Width;
  int Height;
  int Framerate;
};

struct GeneralConfigType
{
  int Level;
  int Length;
  int LineWidth;
  bool Animate;
  float AnimateTime;
  float Angle;
  float StartingRotation;
  bool Center;
  bool Colorful;
  float Saturation;
  int Padding;
};

struct ConfigurationType
{
  WindowConfigType Window;
  GeneralConfigType General;
};

SDL_bool HandleEvents()
{
  SDL_Event event;
  while(SDL_PollEvent(&event) != 0)
  {
    switch (event.type)
    {
      case SDL_QUIT:
        return SDL_TRUE;
      case SDL_WINDOWEVENT:
        switch (event.window.type)
        {
          case SDL_WINDOWEVENT_CLOSE:
            return SDL_TRUE;
        }
        break;
      default:
        return SDL_FALSE;
    }
  }
  return SDL_FALSE;
}

void RedirectLog()
{
  freopen("lsystem.log", "w", stdout);
  freopen("lsystem.log", "w", stderr);
}

void ParseWindowConfiguration(INIReader iniFile, ConfigurationType& config)
{
  config.Window.Width     = iniFile.GetInteger("window", "width", 1600);
  config.Window.Height    = iniFile.GetInteger("window", "height", 900);
  config.Window.Framerate = iniFile.GetInteger("window", "framerate", 60);
  config.Window.Display   = iniFile.GetBoolean("window", "display", true);
}

void ParseGeneralConfiguration(INIReader iniFile, ConfigurationType& config)
{
  config.General.Level            = iniFile.GetInteger("general", "level", 3);
  config.General.Length           = iniFile.GetInteger("general", "length", 5);
  config.General.Angle            = iniFile.GetFloat("general", "angle", 90.0f);
  config.General.Animate          = iniFile.GetBoolean("general", "animate", false);
  config.General.AnimateTime      = iniFile.GetFloat("general", "animatetime", 15.0);
  config.General.StartingRotation = iniFile.GetFloat("general", "startingrotation", 0.0f);
  config.General.LineWidth        = iniFile.GetFloat("general", "linewidth", 1.0f);
  config.General.Center           = iniFile.GetBoolean("general", "center", true);
  config.General.Colorful         = iniFile.GetBoolean("general", "colorful", false);
  config.General.Saturation       = iniFile.GetFloat("general", "saturation", 0.6f);
  config.General.Padding          = iniFile.GetInteger("general", "padding", 20);
}

void ParseSystemConfiguration(INIReader iniFile, LSystem& lSystem)
{
  std::string constants = iniFile.Get("lsystem", "constants", "");
  for (int i = 0; i < constants.size(); ++i)
  {
    std::string actionName = "action" + std::to_string(i);
    std::string action = iniFile.Get("lsystem", actionName, "NO_ACTION");

    if (action == "MOVE_FORWARD")
    {
      lSystem.AddConstant(constants[i], ActionEnum::MOVE_FORWARD);
    }
    else if (action == "DRAW_FORWARD")
    {
      lSystem.AddConstant(constants[i], ActionEnum::DRAW_FORWARD);
    }
    else if (action == "ROTATE_CW")
    {
      lSystem.AddConstant(constants[i], ActionEnum::ROTATE_CW);
    }
    else if (action == "ROTATE_CCW")
    {
      lSystem.AddConstant(constants[i], ActionEnum::ROTATE_CCW);
    }
    else if (action == "PUSH_STATE")
    {
      lSystem.AddConstant(constants[i], ActionEnum::PUSH_STATE);
    }
    else if (action == "POP_STATE")
    {
      lSystem.AddConstant(constants[i], ActionEnum::POP_STATE );
    }
    else
    {
      lSystem.AddConstant(constants[i], ActionEnum::NO_ACTION);
    }
  }

  lSystem.SetAxiom(iniFile.Get("lsystem", "axiom", ""));

  for (int i = 0; i < constants.size(); ++i)
  {
    std::string ruleName = "rule" + std::to_string(i);
    std::string rule = iniFile.Get("lsystem", ruleName, std::string(1, constants[i]));

    lSystem.SetConstantRule(constants[i], rule);
  }
}

bool InitSDL(SDL_Window *&window, SDL_GLContext& gl, const ConfigurationType& config)
{
  int status;

  status = SDL_Init(SDL_INIT_VIDEO);
  if (status != 0)
  {
    std::cerr << "Failed to init video. Error: " << SDL_GetError() << std::endl;
    return false; 
  }

  Uint32 flags = SDL_WINDOW_OPENGL;
  if (!config.Window.Display)
    flags |= SDL_WINDOW_HIDDEN;
    
  std::cout << "Creating window of dimensions " << config.Window.Width << "x" << config.Window.Height << std::endl;
  window = SDL_CreateWindow("Lindenmayer System", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, config.Window.Width, config.Window.Height, flags);
  if (window == NULL)
  {
    std::cerr << "Failed to create window. Error: " << SDL_GetError() << std::endl;
    return false;
  }

  gl = SDL_GL_CreateContext(window);
  if (gl == NULL)
  {
    std::cerr << "Failed to create GL context. Error: " << SDL_GetError() << std::endl;
    return false;
  }

  status = SDL_GL_MakeCurrent(window, gl);
  if (status != 0)
  {
    std::cerr << "Failed to make GL context current. Error: " << SDL_GetError() << std::endl;
    return false;
  }

  status = SDL_GL_SetSwapInterval(1);
  if (status != 0)
  {
    std::cerr << "Failed to set swap interval. Error: " << SDL_GetError() << std::endl;
    return false;
  }

  SDL_bool success = SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
  if (!success)
  {
    std::cerr << "Failed to set render scale quality. Error: " << SDL_GetError() << std::endl;
  }

  status = SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  if (status != 0)
  {
    std::cerr << "Failed to enable double buffer. Error: " << SDL_GetError() << std::endl;
  }

  return true;
}

int main(int argc, char** argv)
{
  RedirectLog();

  std::string iniFile = "example.ini";
  std::string outputFile = "lsystem.bmp";
  std::string animationFolder = "./animation/";
  bool saveFinal = false;
  bool allFrames = false;

  int i = 0;
  do
  {
    std::string opt(argv[i]);
  
    if (opt == "-c" || opt == "--config")
    {
      if (i < argc-1)
      {
        iniFile = std::string(argv[i+1]);
        i += 2;
      }
      else
        ++i;
    }
    else if (opt == "-o" || opt == "--output")
    {
      saveFinal = true;
      if (i < argc-1)
      {
        outputFile = std::string(argv[i+1]);
        i += 2;
      }
      else
        ++i;
    }
    else if (opt == "-a" || opt == "--animation")
    {
      saveFinal = true;
      allFrames = true;
      if (i < argc-1)
      {
        animationFolder = std::string(argv[i+1]);
        i += 2;
      }
      else
        ++i;
    }
    else
      ++i;
  } while (i < argc);

  std::cout << "Getting config from " << iniFile << "." << std::endl;
  if (saveFinal) std::cout << "Saving image to " << outputFile << "." << std::endl;

  INIReader reader(iniFile);
  LSystem lSystem;
  ConfigurationType config;

  ParseWindowConfiguration(reader, config);
  ParseGeneralConfiguration(reader, config);
  ParseSystemConfiguration(reader, lSystem);

  lSystem.Print();

  std::vector<Constant> axiom = lSystem.GenerateNthAxiom(config.General.Level);
  
  int stepsPerFrame = axiom.size();
  if (config.General.AnimateTime > 0.0f)
  {
    stepsPerFrame = (int)std::round(axiom.size() / (config.General.AnimateTime * config.Window.Framerate));
    stepsPerFrame = std::max(stepsPerFrame, 1);
  }

  std::cout << std::endl << config.General.Level << " generation axiom. Length=" << axiom.size() << ". Rendering " << stepsPerFrame << " steps per frame" << std::endl;

  SDL_Window* window;
  SDL_GLContext gl;
  bool success = InitSDL(window, gl, config);

  if (!success)
  {
    exit(-1);
  }

  bool saved = false;
  bool doneRendering = false;
  SDL_bool done = SDL_FALSE;

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  LSystemRenderer LS_Renderer(window, axiom, config.General.Length, config.General.LineWidth, config.General.Angle, config.General.StartingRotation);

  LS_Renderer.SetCenter(config.General.Center);
  LS_Renderer.SetColorful(config.General.Colorful);
  LS_Renderer.SetSaturation(config.General.Saturation);

  if (config.General.Center) LS_Renderer.Center();

  LS_Renderer.SetupGL(config.Window.Display);

  std::filesystem::path animationPath(animationFolder);

  std::filesystem::create_directory(animationPath);

  int frame = 0;
  while(!done)
  {
    bool finishedRenderingThisFrame = false;
    Uint64 start = SDL_GetPerformanceCounter();

    if (!doneRendering)
    {
      glDrawBuffer(GL_BACK);
      glClear(GL_COLOR_BUFFER_BIT);
  
      LS_Renderer.SetupRender();

      if (config.General.Animate)
      {
        doneRendering = finishedRenderingThisFrame = LS_Renderer.RenderNextSteps(stepsPerFrame);
        ++frame;

        if (allFrames)
        {
          std::filesystem::path filename(std::to_string(frame) + ".bmp");
          std::filesystem::path filepath = animationPath / filename;
          SDL_GL_SwapWindow(window);
          LS_Renderer.SaveScreenshot(filepath.string(), config.General.Padding);
          SDL_GL_SwapWindow(window);
        }
      }
      else
      {
        doneRendering = finishedRenderingThisFrame = LS_Renderer.Render();
      }

      glFlush();
      glFinish();
      SDL_GL_SwapWindow(window);
    }
    
    if (doneRendering && !finishedRenderingThisFrame && !saved && (saveFinal || allFrames))
    {
      std::string filepath = outputFile;
      if (allFrames)
      {
        ++frame;
        std::filesystem::path filename(std::to_string(frame) + ".bmp");
        std::filesystem::path p = animationPath / filename;
        filepath = p.string();
      }

      SDL_GL_SwapWindow(window);
      LS_Renderer.SaveScreenshot(filepath, config.General.Padding);
      saved = true;
      SDL_GL_SwapWindow(window);
    }

    done = HandleEvents();

    if (!config.Window.Display)
    {
      if (saveFinal && saved && doneRendering)
        done = SDL_TRUE;

      if(!saveFinal && doneRendering)
        done = SDL_TRUE;
    }

    Uint64 end = SDL_GetPerformanceCounter();
    float elapsedMS = (end - start) / (float)SDL_GetPerformanceFrequency() * 1000.0f;
    float delay = std::max(std::floor((1000.0f / config.Window.Framerate) - elapsedMS), 0.0f);
    SDL_Delay(delay);
  }

  if (gl) SDL_GL_DeleteContext(gl);
  if (window) SDL_DestroyWindow(window);

  exit(0);
}