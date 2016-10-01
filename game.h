#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include "rendering.h"
#include "memory.h"
#include "error_codes.h"

struct Game
{
  const char* name;
  Graphics graphics;
  volatile bool quit;
  
  Buffer main_memory;
  FrameDataBuffer frame_memory;
};

void init_game(Game* game, WindowParams* window_params,
               size_t main_memory_size, size_t frame_memory_size);
void destroy_game(Game* game);

template<typename GameDataType>
void run_game(WindowParams* window_params,
              size_t main_memory_size, size_t frame_memory_size)

{
  ASSERT(GameDataType::on_game_start, "Start callback cannot be null");
  ASSERT(GameDataType::on_game_update, "Update callback cannot be null");
  ASSERT(GameDataType::on_game_finish, "Input callback cannot be null");
  ASSERT(GameDataType::input_callback, "End callback cannot be null");

  Game game;
  GameDataType game_data;
  init_game(&game, window_params, main_memory_size, frame_memory_size);
  
  GameDataType::on_game_start(&game, &game_data);

  SDL_Event e;
  float now = SDL_GetPerformanceCounter();
  float last = 0;
  float dt;
  while (!game.quit)
  {
    buffer_reset(&game.frame_memory);
    last = now;
    now = SDL_GetPerformanceCounter();
    dt = ((now - last) * 1000 / SDL_GetPerformanceFrequency());

    while (SDL_PollEvent(&e) != 0)
    {
      if (e.type == SDL_QUIT)
      {
        game.quit = true;
      }
      GameDataType::input_callback(&game, &game_data, e);
    }    
    GameDataType::on_game_update(&game, &game_data, dt);
  }
  
  GameDataType::on_game_finish(&game, &game_data);
  destroy_game(&game);
}
