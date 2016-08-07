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
  Sprite sprite;
  CommandBuffer render_command_buffer;

  volatile bool quit;

  //non-temp
  const char* name;
  Graphics graphics;
  
  Buffer main_memory;
  FrameDataBuffer render_frame_memory;
  FrameDataBuffer sim_frame_memory;
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
  fatal_game_error(push_buffer(&game->main_memory, &game->render_frame_memory, MEGABYTE(50)));
  fatal_game_error(push_buffer(&game->main_memory, &game->sim_frame_memory, MEGABYTE(50)));
  fatal_game_error(create_command_buffer(&game->render_command_buffer, 1, &game->main_memory,
					 &game->render_frame_memory));

  game->quit = false;
}

void destroy_game(Game* game)
{
  destroy_command_buffer(&game->render_command_buffer);
  fatal_game_error(destroy_graphics(&game->graphics));
  free(game->main_memory.start);
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

int sim_loop(void* data)
{
  Game* game = (Game*) data;
  SDL_Event e;
  float dt = SDL_GetTicks();
  while (!game->quit)
  {
    dt = SDL_GetTicks() - dt;
    sim_update(game, dt, e);
  }
  return 0;
}

static Color screen_clear_color = { 0.0, 0.2, 0.3, 1.0 };
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
	printf("quitto\n");
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
  command_buffer_submit(&game->render_command_buffer, clear_color_dispatch, &screen_clear_color); 
}

void render_loop(Game* game)
{
  while(!game->quit)
  {
    buffer_reset(&game->render_frame_memory);
    render_update(game);
  }
}

void render_update(Game* game)
{
  command_buffer_process(&game->render_command_buffer);
  SDL_GL_SwapWindow(game->graphics.main_window);
}


void run_game(Game* game)
{
  on_game_start(game);
  SDL_DetachThread(SDL_CreateThread(sim_loop, "Sim loop", (void*) game)); 
  render_loop(game);
  on_game_finish(game);
}
