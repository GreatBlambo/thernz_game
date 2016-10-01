#include "game.h"

void init_game(Game* game, WindowParams* window_params,
               size_t main_memory_size, size_t frame_memory_size)
{
  game->name = window_params->name;
  fatal_game_error(init_graphics(&game->graphics, window_params));
  fatal_game_error(create_buffer(&game->main_memory, malloc(main_memory_size), main_memory_size));
  fatal_game_error(create_buffer(&game->frame_memory, malloc(frame_memory_size), frame_memory_size));

  game->quit = false;
}

void destroy_game(Game* game)
{
  fatal_game_error(destroy_graphics(&game->graphics));
  free(game->main_memory.start);
}
