#pragma once

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "tzbackend_dispatch.h"

namespace tz
{
  namespace renderer
  {  
    TZ_GRAPHICS_BACKEND(BackendGL);
  }
}
