#include "render_types.h"

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

namespace tz
{

namespace graphics
{

static SDL_Window* g_main_window = 0;
static SDL_GLContext g_context = 0;
  
GameError init_graphics()
{  // Load modules

  // Init SDL video
  if (SDL_Init(SDL_INIT_VIDEO) < 0)
  {
    printf("SDL_Init Error: %s", SDL_GetError());
    return ERROR_SDL;
  }
  
  // Init SDL image
  int img_flags = IMG_INIT_PNG;
  if(!(IMG_Init(img_flags) & img_flags))
  {
    printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
    return ERROR_SDL;
  }
    
  g_main_window = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 0, 0, SDL_WINDOW_OPENGL);
    
  // Set our OpenGL version.
  // SDL_GL_CONTEXT_CORE gives us only the newer version, deprecated functions are disabled
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    
  // Turn on double buffering with a 24bit Z buffer.
  // You may need to change this to 16 or 32 for your system
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

  SDL_GL_SetSwapInterval(1);
  
  g_context = SDL_GL_CreateContext(g_main_window);
  if (!g_context)
  {
    printf("Failed to create OpenGL context: %s\n", SDL_GetError());
    return ERROR_OPENGL;
  }

  // Init GLEW
  glewExperimental = GL_TRUE;
  GLenum glew_err = glewInit();
  if (glew_err != GLEW_OK)
  {
    printf("Failed to initialize GLEW: %s\n", glewGetErrorString(glew_err));
    return ERROR_GLEW;
  }

  return NO_ERROR;
}

void deinit_graphics()
{
  SDL_GL_DeleteContext(g_context);
  SDL_DestroyWindow(g_main_window);
  SDL_Quit();
}

void set_window_name(const char* name)
{
  SDL_SetWindowTitle(g_main_window, name);
}

void set_window_size(int width, int height)
{
  SDL_SetWindowSize(g_main_window, width, height);
  SDL_SetWindowPosition(g_main_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
}

void set_window_fullscreen()
{
  SDL_SetWindowFullscreen(g_main_window, SDL_WINDOW_FULLSCREEN);
}

void set_window_fullscreen_windowed()
{
  SDL_SetWindowFullscreen(g_main_window, SDL_WINDOW_FULLSCREEN_DESKTOP);
}

void set_window_windowed()
{
  SDL_SetWindowFullscreen(g_main_window, 0);
}

void set_window_position(int x, int y)
{
  SDL_SetWindowPosition(g_main_window, x, y);
}

void swap_backbuffer()
{
  SDL_GL_SwapWindow(g_main_window);
}

}

};
