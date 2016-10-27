#include "tzrendering.h"
#include "tzhash.h"
#include "tzsort.h"

#include "tzbackend_dispatch.h"
#include "tzbackend_types.h"

namespace tz
{

namespace renderer
{

////////////////////////////////////////////////////////////////////////////////
// Module interface
////////////////////////////////////////////////////////////////////////////////

// Any code inside these functions is temporary until we develop a default renderer

void init_rendering()
{
  // init default renderer and backend
  set_backend(TZ_CONFIG_DEFAULT_GFX_BACKEND);
}

void deinit_rendering()
{
  // destroy default renderer
}

void push_frame()
{
  // submit rendering thru default renderer
}

void set_backend(BackendType type)
{
  switch (type)
  {
   case OPENGL:
     BackendGL::bind_commands();
     break;
  }
}

}
}
