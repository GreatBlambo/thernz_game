#pragma once

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "tzbackend_dispatch.h"

#define TZ_BACKEND_DISPATCH_API_IMPL(api, command_name) void api::_##command_name(void* data)

namespace tz
{
  namespace renderer
  {
    class IBackend
    {
    public:
      virtual GameError init() = 0;
      virtual void deinit() = 0;
    
      virtual void set_window_name(const char* name) = 0;
      virtual void set_window_size(int width, int height) = 0;
      virtual void set_window_fullscreen() = 0;
      virtual void set_window_fullscreen_windowed() = 0;
      virtual void set_window_windowed() = 0;
      virtual void set_window_position(int x, int y) = 0;
  
      virtual void swap_backbuffer() = 0;
      virtual void clear_backbuffer(GraphicsBitfield bitfield) = 0;

      // Textures
      virtual GameError load_image_as_texture(int* width, int* height, Texture* texture, const char* pathname) = 0;
      virtual void destroy_texture(Texture* texture) = 0;

      // Shaders
      virtual ShaderID load_shader_source(const char* pathname, ShaderType shader_type) = 0;
      virtual void destroy_shader(ShaderID shader) = 0;
      virtual ShaderProgramID link_shader_program(ShaderID* shaders, size_t num_shaders, const VertexAttribArray& vertex_spec) = 0;
      virtual void destroy_program(ShaderProgramID program) = 0;
      virtual GameError detach_shaders(ShaderProgramID program, ShaderID* shaders, size_t num_shaders) = 0;

    };
  
    class BackendGL : public IBackend
    {
    public:
      TZ_GRAPHICS_BACKEND(BackendGL);
      
      BackendGL();
      
      GameError init();
      void deinit();
    
      void set_window_name(const char* name);
      void set_window_size(int width, int height);
      void set_window_fullscreen();
      void set_window_fullscreen_windowed();
      void set_window_windowed();
      void set_window_position(int x, int y);
    
      void swap_backbuffer();
      void clear_backbuffer(GraphicsBitfield bitfield);

      // Textures
      GameError load_image_as_texture(int* width, int* height, Texture* texture, const char* pathname);
      void destroy_texture(Texture* texture);

      // Shaders
      ShaderID load_shader_source(const char* pathname, ShaderType shader_type);
      void destroy_shader(ShaderID shader);
      ShaderProgramID link_shader_program(ShaderID* shaders, size_t num_shaders, const VertexAttribArray& vertex_spec);
      void destroy_program(ShaderProgramID program);
      GameError detach_shaders(ShaderProgramID program, ShaderID* shaders, size_t num_shaders);
    
    private:
      SDL_Window* m_main_window;
      SDL_GLContext m_context;
    };
  }
}
