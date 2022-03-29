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
#include "ConfigParser.h"

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
  // RedirectLog();

  std::string iniFile = "example.ini";
  std::string outputFile = "lsystem.png";
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

  ConfigurationType config;
  ConfigParser parser(iniFile, config);

  LSystem lSystem;
  lSystem.Configure(config.System);
  lSystem.Print();

  std::vector<LConstant> axiom = lSystem.GenerateNthAxiom(config.General.Generation);
  
  int stepsPerFrame = axiom.size();
  if (config.General.AnimateTime > 0.0f)
  {
    stepsPerFrame = (int)std::round(axiom.size() / (config.General.AnimateTime * config.Window.Framerate));
    stepsPerFrame = std::max(stepsPerFrame, 1);
  }
  int endFrames = (int)std::round(config.General.EndFrameTime * config.Window.Framerate);

  std::cout << std::endl << config.General.Generation << " generation axiom. Length=" << axiom.size() << "." << std::endl;
  if (config.General.Animate) std::cout << "Rendering " << stepsPerFrame << " steps per frame. Lingering on final frame for " << endFrames << " frames." << std::endl;

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

  LSystemRenderer LS_Renderer(window, axiom, config);

  if (config.General.Center) LS_Renderer.Center();

  LS_Renderer.SetupGL(config.Window.Display);
  
  std::filesystem::path animationPath(animationFolder);

  if (allFrames)
  {
    std::filesystem::create_directory(animationPath);
  }

  if (saveFinal)
  {
    std::filesystem::path outputPath(outputFile);
    std::filesystem::create_directories(outputPath.parent_path());
  }

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
          std::filesystem::path filename(std::to_string(frame) + ".png");
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
        --endFrames;
        std::filesystem::path filename(std::to_string(frame) + ".png");
        std::filesystem::path p = animationPath / filename;
        filepath = p.string();
        if (endFrames == 0)
        {
          saved = true;
        }
      }
      else
      {
        saved = true;
      }

      SDL_GL_SwapWindow(window);
      LS_Renderer.SaveScreenshot(filepath, config.General.Padding);
      SDL_GL_SwapWindow(window);

      if (saved)
      {
        std::cout << "Saved resultant curve to PNG." << std::endl;
      }
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