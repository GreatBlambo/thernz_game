#pragma once

#include <GL/glew.h>
#include <SDL2/SDL.h>

#include "error_codes.h"
#include "memory.h"

SDL_Window* create_window(const char* window_name
                          , float width, float height
                          , bool fullscreen
                          , float pos_x = SDL_WINDOWPOS_UNDEFINED, float pos_y = SDL_WINDOWPOS_UNDEFINED);

typedef GLuint TextureID;
TextureID load_image_as_texture(const char* pathname);
void destroy_texture(TextureID texture);

struct Graphics
{
  SDL_GLContext context;
  SDL_Window* main_window;
};

bool graphics_is_valid(Graphics* graphics);
GameError init_graphics(Graphics* graphics
                        , const char* window_name
                        , float width, float height
                        , bool fullscreen
                        , float pos_x = SDL_WINDOWPOS_UNDEFINED, float pos_y = SDL_WINDOWPOS_UNDEFINED);
GameError destroy_graphics(Graphics* graphics);

struct Color
{
  float r;
  float g;
  float b;
  float a;
};

void clear_color(Color* color);
