#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <glm/glm.hpp>

#include "tzgame.h"

namespace tz
{

void Game::init(const size_t main_memory_size, const size_t frame_memory_size)
{
  //TODO
}

void Game::deinit()
{
  //TODO
}

void Game::run(const char* name, const size_t main_memory_size, const size_t frame_memory_size, const bool fullscreen, const int w, const int h, const int x, const int y)
{
  on_game_start();

  InputEvent e;
  float now = SDL_GetPerformanceCounter();
  float last = 0;
  float dt;
  while (!quit)
  {
    last = now;
    now = SDL_GetPerformanceCounter();
    dt = ((now - last) * 1000 / SDL_GetPerformanceFrequency());

    while (poll_events(e) != 0)
    {
      if (e.type == InputEventType::app_close)
      {
        quit = true;
      }
      input_callback(e);
    }    
    on_game_update(dt);
  }
  
  on_game_finish();
  deinit();
}

}
