#pragma once

#include <atomic>

#include "render_functions.h"
#include "graphics.h"
#include "memory.h"

struct SpriteBatch
{
  ShaderProgramID shader_program;
  Texture sprite_atlas;
  int projection_loc;
  int view_loc;

  VertexBufferID quad_vbo;
  VertexArrayID quad_vao;
  IndexArrayID quad_ibo;
  VertexBufferID sprite_vbo;
  VertexBufferID model_vbo;

  float screen_w;
  float screen_h;
};

void create_sprite_batch(SpriteBatch* sprite_batch,
                         float screen_w, float screen_h,
                         Texture sprite_atlas, ShaderProgramID shader_program,
                         size_t max_sprites);
void destroy_sprite_batch(SpriteBatch* sprite_batch);
void render_sprites(SpriteBatch* sprite_batch, Sprite* sprites, glm::mat4* models, size_t num_sprites);
