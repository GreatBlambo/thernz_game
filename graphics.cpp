#include "rendering.h"

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

TextureID load_image_as_texture(const char* pathname)
{
  //Load image at specified path
  SDL_Surface* loaded_surface = IMG_Load(pathname);
  if(loaded_surface == NULL)
  {
    printf("Unable to load image %s! SDL_image Error: %s\n", pathname, IMG_GetError());
    return 0;
  }
  //Create texture from surface pixels
  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  
  int tex_mode = GL_RGB;
  
  if(loaded_surface->format->BytesPerPixel == 4) {
    tex_mode = GL_RGBA;
  }
  
  glTexImage2D(GL_TEXTURE_2D, 0, tex_mode, loaded_surface->w, loaded_surface->h, 0, tex_mode, GL_UNSIGNED_BYTE, loaded_surface->pixels);
  
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  //Clean up surface
  SDL_FreeSurface(loaded_surface);
  
  return texture;
}

void destroy_texture(TextureID texture)
{
  glDeleteTextures(1, &texture);
}

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

  // Init GLEW
  glewExperimental = GL_TRUE;
  if (!glewInit())
  {
    printf("Failed to initialize GLEW\n");
    return ERROR_GLEW;
  }

  graphics->main_window = create_window(window_params->name
                                        , window_params->rect.w, window_params->rect.h
                                        , window_params->fullscreen
                                        , window_params->rect.x, window_params->rect.y);
  graphics->context = SDL_GL_CreateContext(graphics->main_window);
  if (!graphics->context)
  {
    printf("Failed to create OpenGL context\n");
    return ERROR_OPENGL;
  }  
  // Set our OpenGL version.
  // SDL_GL_CONTEXT_CORE gives us only the newer version, deprecated functions are disabled
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  
  // 3.2 is part of the modern versions of OpenGL, but most video cards whould be able to run it
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
  
  // Turn on double buffering with a 24bit Z buffer.
  // You may need to change this to 16 or 32 for your system
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  
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
