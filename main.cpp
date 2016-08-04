#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "rendering.h"
#include "memory.h"
#include "error_codes.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

struct Game
{
  //temp
  Sprite sprite;

  //non-temp
  const char* name;
  Graphics graphics;
  SDL_Event e;
  
  Buffer main_memory;
  Buffer frame_memory;
};

void init_game(Game* game, const char* name
                    , float width, float height
                    , bool fullscreen
                    , float pos_x = SDL_WINDOWPOS_UNDEFINED, float pos_y = SDL_WINDOWPOS_UNDEFINED);
void on_game_start(Game* game);
void on_game_finish(Game* game);
void run_game(Game* game);
bool update_game(Game* game);
void destroy_game(Game* game);

int main(int, char**)
{
  GameError err = NO_ERROR;

  Game game;
  init_game(&game, "Thernz Game"
            , SCREEN_WIDTH
            , SCREEN_HEIGHT
            , false
            , SDL_WINDOWPOS_CENTERED
            , SDL_WINDOWPOS_CENTERED);
  
  on_game_start(&game);
  run_game(&game);
  on_game_finish(&game);
  
  destroy_game(&game);
  return 0;
}

void init_game(Game* game, const char* name
                    , float width, float height
                    , bool fullscreen
                    , float pos_x, float pos_y)
{
  game->name = name;
  WindowParams window_params;
  window_params.name = name;
  window_params.fullscreen = fullscreen;
  window_params.rect.x = pos_x;
  window_params.rect.y = pos_y;
  window_params.rect.w = width;
  window_params.rect.h = height;
  fatal_game_error(init_graphics(&game->graphics, &window_params));

  fatal_game_error(create_buffer(&game->main_memory, malloc(GIGABYTE(1)), GIGABYTE(1)));
}

void on_game_start(Game* game)
{
  // Load assets
  game->sprite.texture = load_image_as_texture("assets/bird.png");
  if(!game->sprite.texture)
    fatal_game_error(ERROR_SDL);
  game->sprite.rect.x = 0;
  game->sprite.rect.y = 0;
  game->sprite.rect.w = 100;
  game->sprite.rect.h = 100;

  ShaderID shaders[2];
  shaders[0] = load_shader_source("assets/shaders/test.vert", GL_VERTEX_SHADER);
  shaders[1] = load_shader_source("assets/shaders/test.frag", GL_FRAGMENT_SHADER);
  if (!shaders[0] || !shaders[1])
    fatal_game_error(ERROR_OPENGL);
  
  game->sprite.shader_program = link_shader_program(shaders, 2);
  if (!game->sprite.shader_program)
    fatal_game_error(ERROR_OPENGL);

  //cleanup  
  detach_shaders(game->sprite.shader_program, shaders, 2); 
  destroy_shader(shaders[0]);
  destroy_shader(shaders[1]);
}

void on_game_finish(Game* game)
{
  destroy_texture(game->sprite.texture);
  destroy_program(game->sprite.shader_program);
}

bool update_game(Game* game)
{
  static Color screen_clear_color = { 0.0, 0.2, 0.3, 1.0 };

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

  
  
  clear_color(&screen_clear_color);
  SDL_GL_SwapWindow(game->graphics.main_window);
  
  return false;
}

void run_game(Game* game)
{
  bool quit = false;
  while(!quit)
  {
    buffer_reset(&game->frame_memory);
    quit = update_game(game);
  }
}


void destroy_game(Game* game)
{
  fatal_game_error(destroy_graphics(&game->graphics));
  free(game->main_memory.start);
}
