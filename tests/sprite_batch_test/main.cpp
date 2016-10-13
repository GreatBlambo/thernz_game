#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <game.h>
#include <sprite_batch.h>
#include <config.h>

#define MAX_SPRITES 1024
#define METERS_PER_PIXEL 1.0f/10.0f

struct SpriteApp : public Game
{
  SpriteBatch sprite_batch;
  Sprite* sprites;
  glm::mat4* transforms;
  size_t num_sprites;

  int num_cols;

  UniformBufferID cam_ubo;
  
  // assets
  Texture texture;
  ShaderProgramID sprite_shader;
  
  void on_game_start();
  void on_game_update(float dt);
  void on_game_finish();
  void input_callback(SDL_Event e);

  void render_update();
};

int main(int, char**)
{
  SpriteApp app;
  app.run(&g_window_params, MAIN_MEMORY_SIZE, FRAME_MEMORY_SIZE);
  return 0;
}

void SpriteApp::on_game_start()
{
  // Load assets
  fatal_game_error(load_image_as_texture(&texture, "assets/super-mario-sprite.png"));

  ShaderID shaders[2];
  shaders[0] = load_shader_source("assets/shaders/test.vert", GL_VERTEX_SHADER);
  shaders[1] = load_shader_source("assets/shaders/test.frag", GL_FRAGMENT_SHADER);
  if (!shaders[0] || !shaders[1])
    fatal_game_error(ERROR_OPENGL);
  
  sprite_shader = link_shader_program(shaders, 2, g_sprite_batch_vert_spec);
  if (!sprite_shader)
    fatal_game_error(ERROR_OPENGL);

  shader_bind_ubo_block(sprite_shader, "CameraData", 0);
  cam_ubo = create_ubo_dynamic(2 * sizeof(glm::mat4), 0);

  glm::mat4 projection = glm::ortho(0.0f, (float) window.rect.w * METERS_PER_PIXEL, (float) window.rect.h * METERS_PER_PIXEL, 0.0f, -1.0f, 1.0f);
  upload_ubo_data(cam_ubo, 0, sizeof(glm::mat4), glm::value_ptr(projection));
  
  glm::mat4 view(1.0f);
  upload_ubo_data(cam_ubo, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
  
  create_sprite_batch(&sprite_batch, MAX_SPRITES);
  
  num_sprites = 100;
  sprites = push_array(Sprite, &main_memory, num_sprites);
  transforms = push_array(glm::mat4, &main_memory, num_sprites);

  num_cols = 10;
  int num_rows = (num_sprites / num_cols);
  int col = 0;
  int row = 0;
  float tex_w = texture.w / num_cols;
  float tex_h = texture.h / num_rows;
  for (size_t i = 0; i < num_sprites; i++)
  {
    col = i % num_cols;
    row = i / num_cols;
    float sprite_x = tex_w * (col) * METERS_PER_PIXEL;
    float sprite_y = tex_h * (row) * METERS_PER_PIXEL;
    float tex_x = tex_w * col;
    float tex_y = tex_h * row;
    float sprite_w = tex_w * METERS_PER_PIXEL;
    float sprite_h = tex_h * METERS_PER_PIXEL;

    float color_index = (float) i/(float) num_sprites;
    create_sprite(&sprites[i], {tex_x, tex_y}, {tex_w, tex_h}, &texture);
      

    glm::mat4 mvm;
    mvm = glm::translate(mvm, {sprite_x , sprite_y, 0.0f});
    mvm = glm::scale(mvm, {sprite_w, sprite_h, 1.0f});
    transforms[i] = mvm;
  }
  
  upload_sprite_batch_data(&sprite_batch, sprites, transforms, num_sprites);
  detach_shaders(sprite_shader, shaders, 2); 
  destroy_shader(shaders[0]);
  destroy_shader(shaders[1]);
}

void SpriteApp::on_game_finish()
{
  destroy_texture(&texture);
  destroy_program(sprite_shader);
  destroy_sprite_batch(&sprite_batch);
  delete_ubo(cam_ubo);
}

void SpriteApp::render_update()
{
  static Color screen_clear_color = { 0.0, 0.2, 0.3 };
  clear_color(&screen_clear_color);

  render_sprites(&sprite_batch, texture, sprite_shader);

  SDL_GL_SwapWindow(graphics.main_window);
}

void SpriteApp::input_callback(SDL_Event e)
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

void SpriteApp::on_game_update(float dt)
{
  upload_sprite_batch_data(&sprite_batch, sprites, transforms, num_sprites);
  render_update();
}
