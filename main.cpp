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

#define MAIN_MEMORY_SIZE GIGABYTE(1)
#define FRAME_MEMORY_SIZE GIGABYTE(1)

#define MAX_SPRITES 1024

struct Game
{
  //components
  Sprite* sprites;
  glm::mat4* sprite_transforms;
  size_t num_sprites;
  size_t max_sprites;

  // systems
  SpriteBatch sprite_batch;

  // assets
  Texture bird_texture;
  ShaderProgramID shader_program;

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
  fatal_game_error(create_buffer(&game->main_memory, malloc(MAIN_MEMORY_SIZE), MAIN_MEMORY_SIZE));
  fatal_game_error(push_buffer(&game->main_memory, &game->frame_memory, FRAME_MEMORY_SIZE));

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
  fatal_game_error(load_image_as_texture(&game->bird_texture, "assets/bird.png"));

  ShaderID shaders[2];
  shaders[0] = load_shader_source("assets/shaders/test.vert", GL_VERTEX_SHADER);
  shaders[1] = load_shader_source("assets/shaders/test.frag", GL_FRAGMENT_SHADER);
  if (!shaders[0] || !shaders[1])
    fatal_game_error(ERROR_OPENGL);
  
  game->shader_program = link_shader_program(shaders, 2);
  if (!game->shader_program)
    fatal_game_error(ERROR_OPENGL);
  
  // Create sprite batch
  create_sprite_batch(&game->sprite_batch,
                      SCREEN_WIDTH, SCREEN_HEIGHT,
                      game->bird_texture, game->shader_program
                      MAX_SPRITES);

  //cleanup  
  detach_shaders(game->shader_program, shaders, 2); 
  destroy_shader(shaders[0]);
  destroy_shader(shaders[1]);

  // Components
  game->sprites = push_array<Sprite>(game->main_memory, MAX_SPRITES);
  game->sprite_transforms = push_array<glm::mat4>(game->main_memory, MAX_SPRITES);
  if (!game->sprites
      || !game->sprite_transforms)
  {
    fatal_error("Not enough memory for data components");
  }

  // Create sprites
  for (size_t i = 0; i < MAX_SPRITES; i++)
  {
    // create_sprite and shit
  };
}

void on_game_finish(Game* game)
{
  destroy_texture(&game->bird_texture);
  destroy_program(game->shader_program);
  destroy_sprite_batch(&game->sprite_batch);
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
  static Color screen_clear_color = { 0.0, 0.2, 0.3 };
  clear_color(&screen_clear_color);
  render_sprites(&game->sprite_batch, &game->sprites, &game->sprite_transforms, 1);
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
