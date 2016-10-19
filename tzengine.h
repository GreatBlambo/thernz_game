#pragma once
#include "tzmemory.h"
#include "tzinput.h"
#include "tzgraphics.h"
#include "tzrendering.h"

namespace tz
{
  enum SystemFlags
  {
    ZERO = 0,
    GRAPHICS = 1,
    RENDERING = 1 << 1
  };
  
  void init(const size_t main_memory_size, const size_t frame_memory_size, int flags);
  void deinit();

  // allocates on the permanent memory buffer
  void* allocate(size_t size, size_t align = TZ_DEFAULT_ALIGN);
  
  // allocates on the per-frame memory buffer that is cleared each frame
  void* frame_allocate(size_t size, size_t align = TZ_DEFAULT_ALIGN);

  struct GameState
  {
    Buffer full_memory;
    Buffer main_memory;
    Buffer frame_memory;
  
    int flags;
  };
  
  extern GameState g_game_state;
  
  // resets the frame memory buffer
  void frame();
}
