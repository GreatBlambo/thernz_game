#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "rendering.h"
#include "memory.h"
#include "error_codes.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define GAME_NAME "Thernz Game"
#define FULLSCREEN false

struct Game
{
  //temp
  Texture bird_texture;
  Sprite sprite;
  ShaderProgramID shader_program;
  SpriteBatch sprite_batch;

  //non-temp
  const char* name;
  Graphics graphics;
  volatile bool quit;
  
  Buffer main_memory;
  FrameDataBuffer frame_memory;
};

void init_game(Game* game, WindowParams* window_params);
void destroy_game(Game* game);
void on_game_start(Game* game);
void on_game_finish(Game* game);
void run_game(Game* game);
int sim_loop(void* data);
void sim_update(Game* game, float dt, SDL_Event e);
void render_loop(Game* game);
void render_update(Game* game);

int main(int, char**)
{
  WindowParams window_params;
  window_params.name = GAME_NAME;
  window_params.fullscreen = FULLSCREEN;
  window_params.rect.x = SDL_WINDOWPOS_CENTERED;
  window_params.rect.y = SDL_WINDOWPOS_CENTERED;
  window_params.rect.w = SCREEN_WIDTH;
  window_params.rect.h = SCREEN_HEIGHT;

  GameError err = NO_ERROR;

  Game game;
  init_game(&game, &window_params);
  run_game(&game);  
  destroy_game(&game);
  return 0;
}

void init_game(Game* game, WindowParams* window_params)
{
  game->name = window_params->name;
  fatal_game_error(init_graphics(&game->graphics, window_params));
  fatal_game_error(create_buffer(&game->main_memory, malloc(GIGABYTE(1)), GIGABYTE(1)));
  fatal_game_error(push_buffer(&game->main_memory, &game->frame_memory, MEGABYTE(50)));

  game->quit = false;
}

void destroy_game(Game* game)
{
  fatal_game_error(destroy_graphics(&game->graphics));
  free(game->main_memory.start);
}

void on_game_start(Game* game)
{
  // Load assets
  check_trace_game_error(load_image_as_texture(&game->bird_texture, "assets/bird.png"));
  if(!texture_is_valid(&game->bird_texture))
    fatal_game_error(ERROR_SDL);

  ShaderID shaders[2];
  shaders[0] = load_shader_source("assets/shaders/test.vert", GL_VERTEX_SHADER);
  shaders[1] = load_shader_source("assets/shaders/test.frag", GL_FRAGMENT_SHADER);
  if (!shaders[0] || !shaders[1])
    fatal_game_error(ERROR_OPENGL);
  
  game->shader_program = link_shader_program(shaders, 2);
  if (!game->shader_program)
    fatal_game_error(ERROR_OPENGL);

  // Create sprites
  create_sprite(&game->sprite, { 0, 0 }, { 100, 100 }, &game->bird_texture);
  transform_sprite(&game->sprite, {500, 500, 0}, {100, 100}, 0, {0, 0, 1});
  
  // Create sprite batch
  create_sprite_batch(&game->sprite_batch, SCREEN_WIDTH, SCREEN_HEIGHT, game->bird_texture, game->shader_program);

  //cleanup  
  detach_shaders(game->shader_program, shaders, 2); 
  destroy_shader(shaders[0]);
  destroy_shader(shaders[1]);
}

void on_game_finish(Game* game)
{
  destroy_texture(&game->bird_texture);
  destroy_program(game->shader_program);
  destroy_sprite_batch(&game->sprite_batch);
}

static Color screen_clear_color = { 0.0, 0.2, 0.3 };
void clear_color_dispatch(void* params, void* data, size_t size)
{
  clear_color((Color*) params);
}

void sim_update(Game* game, float dt, SDL_Event e)
{
  while (SDL_PollEvent(&e) != 0)
  {
    switch(e.type)
    {
    case(SDL_QUIT):
      game->quit = true;
      break;
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
}

void render_update(Game* game)
{
  clear_color(&screen_clear_color);
  render_sprites(&game->sprite_batch, &game->sprite, 1);
  SDL_GL_SwapWindow(game->graphics.main_window);
}

void game_loop(Game* game)
{
  SDL_Event e;
  float dt = SDL_GetTicks();
  while (!game->quit)
  {
    buffer_reset(&game->frame_memory);
    dt = SDL_GetTicks() - dt;
    sim_update(game, dt, e);
    render_update(game);
  }
}

void run_game(Game* game)
{
  on_game_start(game);
  game_loop(game);
  on_game_finish(game);
}
