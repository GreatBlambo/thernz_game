#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include "render_functions.h"
#include "render_types.h"
#include "render_globals.h"
#include "graphics.h"
#include "memory.h"

struct SpriteBatch
{  
  VertexArrayID quad_vao;
  VertexBufferID sprite_vbo;
  VertexBufferID model_vbo;

  size_t num_sprites;
};

void create_sprite_batch(SpriteBatch* sprite_batch, size_t max_sprites);
void upload_sprite_batch_data(SpriteBatch* sprite_batch, Sprite* sprites, glm::mat4* models, size_t num_sprites);
void destroy_sprite_batch(SpriteBatch* sprite_batch);
void render_sprites(SpriteBatch* sprite_batch, const Texture sprite_atlas, ShaderProgramID shader_program);
