#include "tzmemory.h"
#include "tzerror_codes.h"
#include "tzengine.h"
#include "tzrendering.h"

namespace tz
{
GameState g_game_state;

static void init_systems(int flags)
{
  if (flags & RENDERING)
  {
    renderer::init_rendering();
  }
}

static void deinit_systems(int flags)
{
  if (flags & RENDERING)
  {
    renderer::deinit_rendering();
  }
}
  
void init(const size_t main_memory_size, const size_t frame_memory_size, int flags)
{
  size_t total_size = main_memory_size + frame_memory_size;
  fatal_game_error(create_buffer(&g_game_state.full_memory, malloc(total_size), total_size));
    
  fatal_game_error(push_buffer(&g_game_state.full_memory, &g_game_state.main_memory, main_memory_size));
  fatal_game_error(push_buffer(&g_game_state.full_memory, &g_game_state.frame_memory, frame_memory_size));

  g_game_state.flags = flags;
  init_systems(g_game_state.flags);
}

void deinit()
{
  deinit_systems(g_game_state.flags);
  free(g_game_state.full_memory.start);
}

void flush_frame()
{
  buffer_reset(&g_game_state.frame_memory);
}

void frame()
{
  if (g_game_state.flags & RENDERING)
  {
    renderer::push_frame();
  }
  
  flush_frame();
}
  
void* allocate(size_t size, size_t align)
{
  return push_size(&g_game_state.main_memory, size, align);
}
  
void* frame_allocate(size_t size, size_t align)
{
  return push_size(&g_game_state.frame_memory, size, align);
}

}
