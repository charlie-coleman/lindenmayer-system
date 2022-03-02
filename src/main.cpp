#include <iostream>
#include <cmath>

#define SDL_MAIN_HANDLED

#include "glew.h"
#include "GL/GL.h"
#include "SDL_opengl.h"
#include "SDL.h"
#include "INIReader.h"
#include "LSystem.h"
#include "LSystemRenderer.h"

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

int main(int argc, char** argv)
{
  RedirectLog();

  std::string ini_file = "example.ini";
  std::string output_file = "lsystem.bmp";
  bool save = false;

  int i = 0;
  do
  {
    std::string opt(argv[i]);
  
    if (opt == "-c" || opt == "--config")
    {
      if (i < argc-1)
      {
        ini_file = std::string(argv[i+1]);
        i += 2;
      }
      else
        ++i;
    }
    else if (opt == "-o" || opt == "--output")
    {
      save = true;
      if (i < argc-1)
      {
        output_file = std::string(argv[i+1]);
        i += 2;
      }
      else
        ++i;
    }
    else
    {
      ++i;
    }
  } while (i < argc);

  INIReader reader(ini_file);
  
  LSystem lSystem;

  ///
  /// WINDOW
  ///
  int window_width  = reader.GetInteger("window", "width", 1600);
  int window_height = reader.GetInteger("window", "height", 900);
  bool display      = reader.GetBoolean("window", "display", true);
  
  ///
  /// GENERAL
  ///
  int level              = reader.GetInteger("general", "level", 3);
  int length             = reader.GetInteger("general", "length", 5);
  float angle            = reader.GetFloat("general", "angle", 90.0f);
  float startingRotation = reader.GetFloat("general", "startingrotation", 0.0f);
  float linewidth        = reader.GetFloat("general", "linewidth", 1.0f);
  bool center            = reader.GetBoolean("general", "center", true);
  bool colorful          = reader.GetBoolean("general", "colorful", false);
  float saturation       = reader.GetFloat("general", "saturation", 0.6f);

  ///
  /// CONSTANTS
  ///
  std::string constants = reader.Get("lsystem", "constants", "");
  for (int i = 0; i < constants.size(); ++i)
  {
    std::string actionName = "action" + std::to_string(i);
    std::string action = reader.Get("lsystem", actionName, "NO_ACTION");

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

  lSystem.SetAxiom(reader.Get("lsystem", "axiom", ""));

  for (int i = 0; i < constants.size(); ++i)
  {
    std::string ruleName = "rule" + std::to_string(i);
    std::string rule = reader.Get("lsystem", ruleName, std::string(1, constants[i]));

    lSystem.SetConstantRule(constants[i], rule);
  }

  lSystem.Print();

  std::vector<Constant> axiom = lSystem.GenerateNthAxiom(level);

  std::cout << std::endl << level << " generation axiom. Length=" << axiom.size() << std::endl;

  bool saved = false;

  if (SDL_Init(SDL_INIT_VIDEO) == 0)
  {
    Uint32 flags = SDL_WINDOW_OPENGL;
    if (!display)
      flags |= SDL_WINDOW_HIDDEN;

    std::cout << "Creating window of dimensions " << window_width << "x" << window_height << std::endl;
    SDL_Window* window = SDL_CreateWindow("Lindenmayer System", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, window_width, window_height, flags);
    bool doneRendering = false;

    if (window != NULL)
    {
      SDL_bool done = SDL_FALSE;

      SDL_GLContext gl = SDL_GL_CreateContext(window);

      glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
      glClear(GL_COLOR_BUFFER_BIT);

      LSystemRenderer LS_Renderer(window, length, linewidth, angle, startingRotation);

      LS_Renderer.SetCenter(center);
      LS_Renderer.SetColorful(colorful);

      SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

      while(!done)
      {
        Uint64 start = SDL_GetPerformanceCounter();
    
        if (!doneRendering)
        {
          glClear(GL_COLOR_BUFFER_BIT);

          doneRendering = LS_Renderer.Render(axiom, display);

          glFlush();
          SDL_GL_SwapWindow(window);
        }

        if (save && doneRendering && !saved)
        {
          SDL_GL_SwapWindow(window);
          LS_Renderer.SaveScreenshot(output_file);
          saved = true;
          SDL_GL_SwapWindow(window);
        }

        done = HandleEvents();

        if (!display)
        {
          if (save && saved && doneRendering)
            done = SDL_TRUE;

          if(!save && doneRendering)
            done = SDL_TRUE;
        }

        Uint64 end = SDL_GetPerformanceCounter();
        float elapsedMS = (end - start) / (float)SDL_GetPerformanceFrequency() * 1000.0f;
        float delay = std::max(std::floor(16.6667f - elapsedMS), 0.0f);
        SDL_Delay(delay);
      }

      if (gl)
      {
        SDL_GL_DeleteContext(gl);
      }

      if (window)
      {
        SDL_DestroyWindow(window);
      }
    }
    else
    {
      std::cerr << "Failed to create SDL window. Error: " << SDL_GetError() << std::endl;
      return -1;
    }

    SDL_Quit();
  }
  else
  {
    std::cerr << "Failed to initialize SDL." << std::endl;
    return -1;
  }

  return 0;
}