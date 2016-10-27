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
};
  
};
