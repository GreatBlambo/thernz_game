#pragma once

#include <atomic>

#include "render_functions.h"
#include "graphics.h"
#include "memory.h"

struct SpriteBatch
{
  ShaderProgramID shader_program;
  int model_loc;
  int projection_loc;
  int sprite_color_loc;

  VertexBufferID quad_vbo;
  VertexArrayID quad_vao;
  IndexArrayID quad_ibo;

  float screen_w;
  float screen_h;
};

void create_sprite_batch(SpriteBatch* sprite_batch, float screen_w, float screen_h, ShaderProgramID shader_program);
void destroy_sprite_batch(SpriteBatch* sprite_batch);
void render_sprites(SpriteBatch* sprite_batch, Sprite* sprites, size_t num_sprites);
