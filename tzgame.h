#pragma once

#include "tzrendering.h"
#include "tzmemory.h"
#include "tzerror_codes.h"

/**
 * Apps inherit from this struct. As we test features in those apps, we can push them
 * onto this struct as we see fit.
 */

namespace tz
{

struct Game
{
  void run(const char* name, const size_t main_memory_size, const size_t frame_memory_size, const bool fullscreen, const int w, const int h, const int x = SDL_WINDOWPOS_UNDEFINED, const int y = SDL_WINDOWPOS_UNDEFINED);

protected:
  virtual void on_game_start() = 0;
  virtual void on_game_update(float dt) = 0;
  virtual void on_game_finish() = 0;
  virtual void input_callback(InputEvent e) = 0;

  bool quit;
private:
  void init(const size_t main_memory_size, const size_t frame_memory_size);
  void deinit();
};

}
