#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include <game.h>
#include <config.h>

#define MAX_SPRITES 1024

struct SpriteApp
{
  SpriteBatch sprite_batch;
  Sprite* sprites;
  glm::mat4* transforms;
  size_t num_sprites;

  int num_cols;
  
  // assets
  Texture bird_texture;
  ShaderProgramID sprite_shader;
  
  static void on_game_start(Game* game, SpriteApp* game_data);
  static void on_game_update(Game* game, SpriteApp* game_data, float dt);
  static void on_game_finish(Game* game, SpriteApp* game_data);
  static void input_callback(Game* game, SpriteApp* game_data, SDL_Event e);
};

int main(int, char**)
{
  run_game<SpriteApp>(&g_window_params, MAIN_MEMORY_SIZE, FRAME_MEMORY_SIZE);
  return 0;
}

void SpriteApp::on_game_start(Game* game, SpriteApp* game_data)
{
  // Load assets
  fatal_game_error(load_image_as_texture(&game_data->bird_texture, "assets/bird.png"));

  ShaderID shaders[2];
  shaders[0] = load_shader_source("assets/shaders/test.vert", GL_VERTEX_SHADER);
  shaders[1] = load_shader_source("assets/shaders/test.frag", GL_FRAGMENT_SHADER);
  if (!shaders[0] || !shaders[1])
    fatal_game_error(ERROR_OPENGL);
  
  game_data->sprite_shader = link_shader_program(shaders, 2, g_sprite_batch_vert_spec);
  if (!game_data->sprite_shader)
    fatal_game_error(ERROR_OPENGL);
  
  create_sprite_batch(&game_data->sprite_batch,
                      SCREEN_WIDTH, SCREEN_HEIGHT,
                      game_data->bird_texture, game_data->sprite_shader,
                      MAX_SPRITES);
  
  game_data->num_sprites = 100;
  game_data->sprites = push_array<Sprite>(&game->main_memory, game_data->num_sprites);
  game_data->transforms = push_array<glm::mat4>(&game->main_memory, game_data->num_sprites);

  game_data->num_cols = 10;
  int num_rows = (game_data->num_sprites / game_data->num_cols);
  int col = 0;
  int row = 0;
  float sprite_w = SCREEN_WIDTH / game_data->num_cols;
  float sprite_h = SCREEN_HEIGHT / num_rows;
  float tex_w = game_data->bird_texture.w / game_data->num_cols;
  float tex_h = game_data->bird_texture.h / num_rows;
  for (size_t i = 0; i < game_data->num_sprites; i++)
  {
    col = i % game_data->num_cols;
    row = i / game_data->num_cols;
    float sprite_x = sprite_w * (col);
    float sprite_y = sprite_h * (row);
    float tex_x = tex_w * col;
    float tex_y = tex_h * row;

    float color_index = (float) i/(float) game_data->num_sprites;
    create_sprite(&game_data->sprites[i], {tex_x, tex_y}, {tex_w, tex_h},
                  &game_data->bird_texture, {color_index, 1 - color_index, color_index});
      

    glm::mat4 mvm;
    mvm = glm::translate(mvm, {sprite_x, sprite_y, 0.0f});
    mvm = glm::scale(mvm, {sprite_w, sprite_h, 1.0f});
    game_data->transforms[i] = mvm;
  }
  
  upload_sprite_batch_data(&game_data->sprite_batch, game_data->sprites, game_data->transforms, game_data->num_sprites);
  detach_shaders(game_data->sprite_shader, shaders, 2); 
  destroy_shader(shaders[0]);
  destroy_shader(shaders[1]);
}

void SpriteApp::on_game_finish(Game* game, SpriteApp* game_data)
{
  destroy_texture(&game_data->bird_texture);
  destroy_program(game_data->sprite_shader);
  destroy_sprite_batch(&game_data->sprite_batch);
}

void render_update(Graphics* graphics, SpriteApp* game_data)
{
  static Color screen_clear_color = { 0.0, 0.2, 0.3 };
  static glm::mat4 view;
  clear_color(&screen_clear_color);

  render_sprites(&game_data->sprite_batch, view);

  SDL_GL_SwapWindow(graphics->main_window);
}

void SpriteApp::input_callback(Game* game, SpriteApp* game_data, SDL_Event e)
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

void SpriteApp::on_game_update(Game* game, SpriteApp* game_data, float dt)
{
  upload_sprite_batch_data(&game_data->sprite_batch, game_data->sprites, game_data->transforms, game_data->num_sprites);
  render_update(&game->graphics, game_data);
}
