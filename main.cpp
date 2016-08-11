#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

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
  // temp
  SpriteBatch sprite_batch;
  Sprite* sprites;
  glm::mat4* transforms;
  size_t num_sprites;

  int num_cols;
  
  // assets
  Texture bird_texture;
  ShaderProgramID sprite_shader;

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
  fatal_game_error(create_buffer(&game->frame_memory, malloc(FRAME_MEMORY_SIZE), FRAME_MEMORY_SIZE));

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
  
  game->sprite_shader = link_shader_program(shaders, 2, g_sprite_batch_vert_spec);
  if (!game->sprite_shader)
    fatal_game_error(ERROR_OPENGL);
  
  create_sprite_batch(&game->sprite_batch,
                      SCREEN_WIDTH, SCREEN_HEIGHT,
                      game->bird_texture, game->sprite_shader,
                      MAX_SPRITES);
  
  game->num_sprites = 100;
  game->sprites = push_array<Sprite>(&game->main_memory, game->num_sprites);
  game->transforms = push_array<glm::mat4>(&game->main_memory, game->num_sprites);

  game->num_cols = 10;
  int num_rows = (game->num_sprites / game->num_cols);
  int col = 0;
  int row = 0;
  float sprite_w = SCREEN_WIDTH / game->num_cols;
  float sprite_h = SCREEN_HEIGHT / num_rows;
  for (size_t i = 0; i < game->num_sprites; i++)
  {
    col = i % game->num_cols;
    row = i / game->num_cols;
    float sprite_x = sprite_w * (col);
    float sprite_y = sprite_h * (row);
    float tex_w = game->bird_texture.w / game->num_cols;
    float tex_h = game->bird_texture.h / num_rows;
    float tex_x = tex_w * (col);
    float tex_y = tex_h * (row);
    glm::mat4 mvm;
    
    create_sprite(&game->sprites[i], {tex_x, tex_y}, {tex_w, tex_h},
                  &game->bird_texture);
    mvm = glm::translate(mvm, {sprite_x, sprite_y, 0.0f});
    game->transforms[i] = glm::scale(mvm, {sprite_w, sprite_h, 1.0f});
  }
  
  upload_sprite_batch_data(&game->sprite_batch, game->sprites, game->transforms, game->num_sprites);
  
  //cleanup  
  detach_shaders(game->sprite_shader, shaders, 2); 
  destroy_shader(shaders[0]);
  destroy_shader(shaders[1]);
}

void on_game_finish(Game* game)
{
  destroy_texture(&game->bird_texture);
  destroy_program(game->sprite_shader);
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
  upload_sprite_batch_data(&game->sprite_batch, game->sprites, game->transforms, game->num_sprites);
}

void render_update(Game* game)
{
  static Color screen_clear_color = { 0.0, 0.2, 0.3 };
  static glm::mat4 view(1.0f);
  clear_color(&screen_clear_color);

  render_sprites(&game->sprite_batch, view);

  SDL_GL_SwapWindow(game->graphics.main_window);
}

void game_loop(Game* game)
{
  SDL_Event e;
  float now = SDL_GetPerformanceCounter();
  float last = 0;
  float dt;
  while (!game->quit)
  {
    buffer_reset(&game->frame_memory);
    last = now;
    now = SDL_GetPerformanceCounter();
    dt = ((now - last) * 1000 / SDL_GetPerformanceFrequency());
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
