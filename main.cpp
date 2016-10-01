#include <game.h>
#include <config.h>

#include <thirdparty/tinyobjloader/tiny_obj_loader.h>

struct ThernzGame : public Game
{
  void on_game_start();
  void on_game_update(float dt);
  void on_game_finish();
  void input_callback(SDL_Event e);
};

void ThernzGame::on_game_start()
{
}

void ThernzGame::on_game_update(float dt)
{
}

void ThernzGame::on_game_finish()
{
}

void ThernzGame::input_callback(SDL_Event e)
{
  switch(e.type)
  {
  case(SDL_KEYDOWN):
  {
    switch(e.key.keysym.sym)
    {
    case(SDLK_ESCAPE):
      printf("quit\n");
      quit = true;
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

int main(int, char**)
{
  ThernzGame game;
  game.run(&g_window_params, MAIN_MEMORY_SIZE, FRAME_MEMORY_SIZE);
  return 0;
}
