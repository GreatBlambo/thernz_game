#include "graphics.h"
#include "render_types.h"

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

////////////////////////////////////////////////////////////////////////////////
// Windows and Graphics
////////////////////////////////////////////////////////////////////////////////

SDL_Window* create_window(const char* window_name
                          , float width, float height
                          , bool fullscreen
                          , float pos_x, float pos_y)
{
  const char* name = "";
  if (name)
    name = window_name;
  
  Uint32 flags = SDL_WINDOW_OPENGL;
  if (fullscreen)
    flags = flags | SDL_WINDOW_FULLSCREEN;
  
  return SDL_CreateWindow(name,
                          pos_x, pos_y,
                          width, height,
                          flags);
}

bool graphics_is_valid(Graphics* graphics)
{
  return (graphics && graphics->main_window && graphics->context);
}

GameError init_graphics(Graphics* graphics, WindowParams* window_params)
{
  // Sanitize params
  if (!graphics || !window_params)
    return ERROR_NULL_PARAM;
  
  // Load modules

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

  graphics->main_window = create_window(window_params->name
                                        , window_params->rect.w, window_params->rect.h
                                        , window_params->fullscreen
                                        , window_params->rect.x, window_params->rect.y);
  
  // Set our OpenGL version.
  // SDL_GL_CONTEXT_CORE gives us only the newer version, deprecated functions are disabled
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
 
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  
  // Turn on double buffering with a 24bit Z buffer.
  // You may need to change this to 16 or 32 for your system
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  
  graphics->context = SDL_GL_CreateContext(graphics->main_window);
  if (!graphics->context)
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

  //glEnable(GL_BLEND);
  //glEnable(GL_DEPTH_TEST);
  //glDepthFunc(GL_LESS);

  return NO_ERROR;
}

GameError destroy_graphics(Graphics* graphics)
{
  if (!graphics_is_valid(graphics))
    return ERROR_INVALID_PARAM;
  SDL_GL_DeleteContext(graphics->context);
  SDL_DestroyWindow(graphics->main_window);
  SDL_Quit();
  return NO_ERROR;
}
