#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include "rendering.h"
#include "memory.h"
#include "error_codes.h"

/**
 * Apps inherit from this struct. As we test features in those apps, we can push them
 * onto this struct as we see fit.
 */

struct Game
{
  void run(WindowParams* window_params,
           size_t main_memory_size, size_t frame_memory_size);

protected:
  virtual void on_game_start() = 0;
  virtual void on_game_update(float dt) = 0;
  virtual void on_game_finish() = 0;
  virtual void input_callback(SDL_Event e) = 0;

  const char* name;
  Graphics graphics;
  volatile bool quit;
  
  Buffer main_memory;
  FrameDataBuffer frame_memory;
  
private:
  void init(const char* name, const size_t main_memory_size, const size_t frame_memory_size, const float w, bool fullscreen, const float h, const float x = SDL_WINDOWPOS_UNDEFINED, const float y = SDL_WINDOWPOS_UNDEFINED);
  void init(WindowParams* window_params, const size_t main_memory_size, const size_t frame_memory_size);
  void deinit();
};
