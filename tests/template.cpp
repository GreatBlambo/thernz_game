#include <game.h>
#include <config.h>

struct App : public Game
{
  void on_game_start();
  void on_game_update(float dt);
  void on_game_finish();
  void input_callback(SDL_Event e);
};

void App::on_game_start()
{
}

void App::on_game_update(float dt)
{
}

void App::on_game_finish()
{
}

void App::input_callback(SDL_Event e)
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
  App game;
  game.run(&g_window_params, MAIN_MEMORY_SIZE, FRAME_MEMORY_SIZE);
  return 0;
}
