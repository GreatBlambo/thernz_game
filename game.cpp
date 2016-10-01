#include "game.h"

void Game::init(WindowParams* window_params,
           size_t main_memory_size, size_t frame_memory_size)
{
  name = window_params->name;
  fatal_game_error(init_graphics(&graphics, window_params));
  fatal_game_error(create_buffer(&main_memory, malloc(main_memory_size), main_memory_size));
  fatal_game_error(create_buffer(&frame_memory, malloc(frame_memory_size), frame_memory_size));

  quit = false;
}

void Game::deinit()
{
  fatal_game_error(destroy_graphics(&graphics));
  free(main_memory.start);
}

void Game::run(WindowParams* window_params,
               size_t main_memory_size, size_t frame_memory_size)
{
  init(window_params, main_memory_size, frame_memory_size);
  on_game_start();

  SDL_Event e;
  float now = SDL_GetPerformanceCounter();
  float last = 0;
  float dt;
  while (!quit)
  {
    buffer_reset(&frame_memory);
    last = now;
    now = SDL_GetPerformanceCounter();
    dt = ((now - last) * 1000 / SDL_GetPerformanceFrequency());

    while (SDL_PollEvent(&e) != 0)
    {
      if (e.type == SDL_QUIT)
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
