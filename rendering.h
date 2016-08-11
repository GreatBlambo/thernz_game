#pragma once

#include <atomic>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include "render_functions.h"
#include "graphics.h"
#include "memory.h"

#define SPRITE_ATTRIB_POSITION 0
#define SPRITE_ATTRIB_COLOR 1
#define SPRITE_ATTRIB_SPRITE_UV 2
#define SPRITE_ATTRIB_MODEL 3

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

  size_t num_sprites;

  float screen_w;
  float screen_h;
};

void create_sprite_batch(SpriteBatch* sprite_batch,
                         float screen_w, float screen_h,
                         Texture sprite_atlas, ShaderProgramID shader_program,
                         size_t max_sprites);
void upload_sprite_batch_data(SpriteBatch* sprite_batch, Sprite* sprites, glm::mat4* models, size_t num_sprites);
void destroy_sprite_batch(SpriteBatch* sprite_batch);
void render_sprites(SpriteBatch* sprite_batch, glm::mat4 view);
