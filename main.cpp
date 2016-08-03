#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "rendering.h"
#include "error_codes.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

////////////////////////////////////////////////////////////////////////////////
// TODO ////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/*******************************************************************************
 * 1. Finish off sprite rendering and the renderer.
 * 2. GameApi struct that contains function pointers for when the game starts,
 *    when the game finishes, and when it's updating.
 * 3. Clean up the code a bit.
/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////

struct Game
{
  const char* name;
  Graphics graphics;
  SDL_Event e;
  void* state_data;
};

GameError init_game(Game* game, const char* name
                    , float width, float height
                    , bool fullscreen
                    , float pos_x = SDL_WINDOWPOS_UNDEFINED, float pos_y = SDL_WINDOWPOS_UNDEFINED);
GameError on_game_start(Game* game);
GameError on_game_finish(Game* game);
GameError run_game(Game* game);
bool update_game(Game* game);
GameError destroy_game(Game* game);

int main(int, char**)
{
  GameError err = NO_ERROR;

  Game game;
  CHECK_ERR_TRACE_RETURN(err, init_game(&game, "Thernz Game"
                                    , SCREEN_WIDTH
                                    , SCREEN_HEIGHT
                                    , false
                                    , SDL_WINDOWPOS_CENTERED
                                    , SDL_WINDOWPOS_CENTERED));
  
  CHECK_ERR_TRACE_RETURN(err, on_game_start(&game));
  CHECK_ERR_TRACE_RETURN(err, run_game(&game));
  CHECK_ERR_TRACE_RETURN(err, on_game_finish(&game));
  
  CHECK_ERR_TRACE_RETURN(err, destroy_game(&game));
  return 0;
}

GameError init_game(Game* game, const char* name
                    , float width, float height
                    , bool fullscreen
                    , float pos_x, float pos_y)
{
  if (!game || !name)
    return ERROR_NULL_PARAM;
  GameError err = NO_ERROR;
  game->name = name;
  WindowParams window_params;
  window_params.name = name;
  window_params.fullscreen = fullscreen;
  window_params.rect.x = pos_x;
  window_params.rect.y = pos_y;
  window_params.rect.w = width;
  window_params.rect.h = height;
  if (err = init_graphics(&game->graphics, &window_params))
    return err;
  return err;
}

struct GameData
{
  TextureID stork_image;
  Renderer renderer;
  Buffer main_memory;
};

GameError on_game_start(Game* game)
{
  GameError err = NO_ERROR;
  Buffer main_memory;
  
  if (err = create_buffer(&main_memory, malloc(GIGABYTE(1)), GIGABYTE(1))) return err;
  
  GameData* game_data = push_struct<GameData>(&main_memory);
  if (!game_data)
    return ERROR_ALLOC_FAIL;

  game_data->stork_image = load_image_as_texture("assets/bird.png");
  if (!game_data->stork_image)
    return ERROR_SDL;

  game_data->main_memory = main_memory;
  if (err = create_renderer(&game_data->renderer, &game_data->main_memory, 2048, MEGABYTE(50))) return err;

  game->state_data = (void*) game_data;  
  return err;
}

GameError on_game_finish(Game* game)
{
  if (!game)
    return ERROR_NULL_PARAM;
  GameData* game_data = (GameData*) game->state_data;
  if (!game_data)
    return ERROR_NULL_PARAM;

  destroy_texture(game_data->stork_image);
  free(game_data);

  return NO_ERROR;
}

GameError run_game(Game* game)
{
  bool quit = false;
  while(!quit)
  {
    quit = update_game(game);
  }
  
  return NO_ERROR;
}

bool update_game(Game* game)
{
  static Color screen_clear_color = { 0.0, 0.2, 0.3, 1.0 };

  GameData* game_data = (GameData*) game->state_data;
  if (!game_data)
    return ERROR_NULL_PARAM;

  while (SDL_PollEvent(&game->e) != 0)
  {
    switch(game->e.type)
    {
    case(SDL_QUIT):
      return true;
    break;
    case(SDL_KEYDOWN):
    {
      if (game->e.key.keysym.sym == SDLK_ESCAPE)
        return true;
    }
    break;
    default:
      break;
    }
  }
  render(&game_data->renderer);
  clear_color(&screen_clear_color);
  SDL_GL_SwapWindow(game->graphics.main_window);
  return false;
}

GameError destroy_game(Game* game)
{
  GameError err = NO_ERROR;
  CHECK_ERR_RETURN(err, destroy_graphics(&game->graphics));
  return err;
}
