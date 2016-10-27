#pragma once

#include <glm/glm.hpp>
#include <thirdparty/bitsquid-foundation-git/array.h>

#include "tzengine.h"
#include "tzconfig.h"
#include "tzrender_types.h"
#include "tzsort.h"
#include "tzbackend_dispatch.h"
#include "tzbackend_types.h"
#include "tzcommand_buffer.h"

namespace tz
{
  
namespace renderer
{
  ////////////////////////////////////////////////////////////////////////////////
  // Module interface
  ////////////////////////////////////////////////////////////////////////////////

  enum BackendType
  {
    OPENGL
  };
  
  void init_rendering();
  void deinit_rendering();
  void push_frame();
  void set_backend(BackendType type);

  void set_window_name(const char* name);
  void set_window_size(int width, int height);
  void set_window_fullscreen();
  void set_window_fullscreen_windowed();
  void set_window_windowed();
  void set_window_position(int x, int y);

  void swap_backbuffer();
  void clear_backbuffer(GraphicsBitfield bitfield);

  GameError load_image_as_texture(int* width, int* height, Texture* texture, const char* pathname);
  void destroy_texture(Texture* texture);
  ShaderID load_shader_source(const char* pathname, ShaderType shader_type);
  void destroy_shader(ShaderID shader);
  ShaderProgramID link_shader_program(ShaderID* shaders, size_t num_shaders, const VertexAttribArray& vertex_spec);
  void destroy_program(ShaderProgramID program);
  GameError detach_shaders(ShaderProgramID program, ShaderID* shaders, size_t num_shaders);
};
  
};
