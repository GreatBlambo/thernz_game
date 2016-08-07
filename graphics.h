#pragma once

#include <GL/glew.h>
#include <SDL2/SDL.h>

#include "error_codes.h"
#include "memory.h"
#include "render_types.h"
#include "command_buffer.h"

struct WindowParams
{
  const char* name;
  SDL_Rect rect;
  bool fullscreen;
};

struct Graphics
{
  SDL_GLContext context;
  SDL_Window* main_window;
};

////////////////////////////////////////////////////////////////////////////////
// Assets
////////////////////////////////////////////////////////////////////////////////

// Textures
TextureID load_image_as_texture(const char* pathname);
void destroy_texture(TextureID texture);

// Shaders
ShaderID load_shader_source(const char* pathname, GLenum shader_type);
void destroy_shader(ShaderID shader);
ShaderProgramID link_shader_program(ShaderID* shaders, size_t num_shaders);
void destroy_program(ShaderProgramID program);
GameError detach_shaders(ShaderProgramID program, ShaderID* shaders, size_t num_shaders);

////////////////////////////////////////////////////////////////////////////////
// Windows and Graphics
////////////////////////////////////////////////////////////////////////////////

SDL_Window* create_window(const char* window_name, float width, float height, bool fullscreen, float pos_x = SDL_WINDOWPOS_UNDEFINED, float pos_y = SDL_WINDOWPOS_UNDEFINED);

bool graphics_is_valid(Graphics* graphics);
GameError init_graphics(Graphics* graphics, WindowParams* window_params);
GameError destroy_graphics(Graphics* graphics);
