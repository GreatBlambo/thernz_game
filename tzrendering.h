#pragma once

#include <stdint.h>
#include <algorithm>

#include <glm/glm.hpp>

#include "tzengine.h"
#include "tzconfig.h"
#include "tzqueue.h"
#include "tzrender_types.h"

namespace tz
{
/**
 * Sorts and submits draw calls to the GPU. That's all this module does for now. We can sort out
 * solutions for thread safety and double buffering later.
 * 
 */
namespace renderer
{
  /**
   * Note: It's completely unecessary to sort by vao. Sorting by material groups draw calls with the
   * same vao together naturally. Additionally, by sharing buffers you can increase the chances
   * of not needing a vao switch.
   */
  typedef uint64_t Key;
  
  typedef uint32_t MaterialID;
  MaterialID create_material_id(ShaderProgramID program, TextureID* textures, size_t num_textures);
  
  ////////////////////////////////////////////////////////////////////////////////
  // CommandBuffer TODO
  ////////////////////////////////////////////////////////////////////////////////
  
  class CommandBuffer
  {
  };

  ////////////////////////////////////////////////////////////////////////////////
  // Module interface
  ////////////////////////////////////////////////////////////////////////////////
  
  void init_rendering();
  void deinit_rendering();
  void push_frame();
};
  
};
