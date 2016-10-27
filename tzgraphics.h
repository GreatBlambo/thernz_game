#pragma once

#include "tzrender_types.h"

namespace tz
{
  namespace graphics
  {
    
    GameError init_graphics();
    void deinit_graphics();
  
    void set_window_name(const char* name);
    void set_window_size(int width, int height);
    void set_window_fullscreen();
    void set_window_fullscreen_windowed();
    void set_window_windowed();

    void swap_backbuffer();

  
    ////////////////////////////////////////////////////////////////////////////////
    // Assets
    ////////////////////////////////////////////////////////////////////////////////

    // Textures

    GameError load_image_as_texture(int* width, int* height, Texture* texture, const char* pathname);

    void destroy_texture(Texture* texture);
    ShaderID load_shader_source(const char* pathname, ShaderType shader_type);

    void destroy_shader(ShaderID shader);

    ShaderProgramID link_shader_program(ShaderID* shaders, size_t num_shaders, const VertexAttribArray& vertex_spec);

    void destroy_program(ShaderProgramID program);

    GameError detach_shaders(ShaderProgramID program, ShaderID* shaders, size_t num_shaders);
  }
}
