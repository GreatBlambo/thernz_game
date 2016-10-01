#include <game.h>
#include <config.h>

#include <thirdparty/tinyobjloader/tiny_obj_loader.h>

struct ThernzGame
{
  static void on_game_start(Game* game, ThernzGame* game_data);
  static void on_game_update(Game* game, ThernzGame* game_data, float dt);
  static void on_game_finish(Game* game, ThernzGame* game_data);
  static void input_callback(Game* game, ThernzGame* game_data, SDL_Event e);
};

int main(int, char**)
{
  run_game<ThernzGame>(&g_window_params, MAIN_MEMORY_SIZE, FRAME_MEMORY_SIZE);
  return 0;
}

void ThernzGame::on_game_start(Game* game, ThernzGame* game_data)
{
}

void ThernzGame::on_game_update(Game* game, ThernzGame* game_data, float dt)
{
}

void ThernzGame::on_game_finish(Game* game, ThernzGame* game_data)
{
}

void ThernzGame::input_callback(Game* game, ThernzGame* game_data, SDL_Event e)
{
  switch(e.type)
  {
  case(SDL_KEYDOWN):
  {
    switch(e.key.keysym.sym)
    {
    case(SDLK_ESCAPE):
      printf("quit\n");
      game->quit = true;
      break;
    case(SDLK_f):
      printf("squad\n");
      break;
    default:
      break;
    }
  }
  break;
  default:
    break;
  }
}
