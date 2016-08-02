#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "rendering.h"
#include "error_codes.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

struct Game
{
  const char* name;
  Graphics graphics;
};

GameError init_game(Game* game, const char* name
                    , float width, float height
                    , bool fullscreen
                    , float pos_x = SDL_WINDOWPOS_UNDEFINED, float pos_y = SDL_WINDOWPOS_UNDEFINED);
GameError update_game(Game* game);
GameError destroy_game(Game* game);

int main(int, char**)
{
  GameError err = NO_ERROR;

  Game game;
  if (err = init_game(&game, "Thernz Game"
                      , SDL_WINDOWPOS_CENTERED
                      , SDL_WINDOWPOS_CENTERED
                      , false
                      , SCREEN_WIDTH
                      , SCREEN_HEIGHT))
    return err;
  
  TextureID stork_image = load_image_as_texture("assets/bird.png");
  if (!stork_image)
    return ERROR_SDL;

  Color screen_clear_color;
  screen_clear_color.r = 0.0;
  screen_clear_color.g = 0.2;
  screen_clear_color.b = 0.3;
  screen_clear_color.a = 1.0;

  clear_color(&screen_clear_color);
  
  bool quit = false;
  SDL_Event e;
  while (!quit)
  {
    while (SDL_PollEvent(&e) != 0)
    {
      if (e.type == SDL_QUIT)
        quit = true;
      if (e.type == SDL_KEYDOWN)
      {
        if (e.key.keysym.sym == SDLK_ESCAPE)
          quit = true;
      }
    }
    clear_color(&screen_clear_color);
    update_game(&game);
  }
  destroy_texture(stork_image);
  destroy_game(&game);
  return 0;
}

GameError init_game(Game* game, const char* name
                    , float width, float height
                    , bool fullscreen
                    , float pos_x, float pos_y)
{
  if (!game || !name)
    return ERROR_NULL_PARAM;
  if (!graphics_is_valid(&game->graphics))
    return ERROR_INVALID_PARAM;
  GameError err = NO_ERROR;
  game->name = name;
  if (err = init_graphics(&game->graphics
                          , name
                          , width, height
                          , fullscreen
                          , pos_x, pos_y))
    return err;
  return err;
}

GameError update_game(Game* game)
{
  SDL_GL_SwapWindow(game->graphics.main_window);
}

GameError destroy_game(Game* game)
{
  destroy_graphics(&game->graphics);
}
