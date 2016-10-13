#include "memory.h"
#include "error_codes.h"
#include "tzengine.h"
#include "tzgraphics.h"

namespace tz
{
  static Buffer g_full_memory;
  static Buffer g_main_memory;
  static FrameDataBuffer g_frame_memory;
  
  static SystemFlags g_flags;

  static void init_systems(SystemFlags flags)
  {
    if (flags & GRAPHICS)
    {
      init_graphics();
    }
  }

  static void deinit_systems(SystemFlags flags)
  {
    if (flags & GRAPHICS)
    {
      deinit_graphics();
    }
  }
  
  void init(const size_t main_memory_size, const size_t frame_memory_size, SystemFlags flags)
  {
    size_t total_size = main_memory_size + frame_memory_size;
    fatal_game_error(create_buffer(&g_full_memory, malloc(total_size), total_size));
    
    fatal_game_error(push_buffer(&g_full_memory, &g_main_memory, main_memory_size));
    fatal_game_error(push_buffer(&g_full_memory, &g_frame_memory, frame_memory_size));

    g_flags = flags;
    init_systems(g_flags);
  }

  void deinit()
  {
    deinit_systems(g_flags);
    free(g_full_memory.start);
  }
  
  inline void* allocate(size_t size, size_t align)
  {
    return push_size(&g_main_memory, size, align);
  }
  
  inline void* frame_allocate(size_t size, size_t align)
  {
    return push_size(&g_frame_memory, size, align);
  }

  void flush_frame()
  {
    buffer_reset(&g_frame_memory);
  }
}
