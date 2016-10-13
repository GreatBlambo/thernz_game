#pragma once
#include "memory.h"

#include "tzinput.h"

#include "tzgraphics.h"

namespace tz
{
  enum SystemFlags
  {
    ZERO,
    GRAPHICS
  };
  
  void init(const size_t main_memory_size, const size_t frame_memory_size, SystemFlags flags);
  void deinit();

  // allocates on the permanent memory buffer
  inline void* allocate(size_t size, size_t align = DEFAULT_ALIGN);
  // allocates on the per-frame memory buffer that is cleared each frame
  inline void* frame_allocate(size_t size, size_t align = DEFAULT_ALIGN);
  
  // resets the frame memory buffer
  void flush_frame();
}
