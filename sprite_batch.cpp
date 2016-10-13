#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "sprite_batch.h"
#include "render_types.h"

#include "render_globals.h"

////////////////////////////////////////////////////////////////////////////////
// Sprite batching
////////////////////////////////////////////////////////////////////////////////

void create_sprite_batch(SpriteBatch* sprite_batch, size_t max_sprites)
{
  // Set parameters
  sprite_batch->num_sprites = 0;
  
  // Initialize buffers
  glGenVertexArrays(1, &sprite_batch->quad_vao);
  glGenBuffers(1, &sprite_batch->sprite_vbo);
  glGenBuffers(1, &sprite_batch->model_vbo);

  // Allocate vbos
  glBindBuffer(GL_ARRAY_BUFFER, sprite_batch->sprite_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Sprite) * max_sprites, NULL, GL_DYNAMIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, sprite_batch->model_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * max_sprites, NULL, GL_DYNAMIC_DRAW);
  
  // Set vertex attributes
  glBindVertexArray(sprite_batch->quad_vao);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_quad_ibo);
  
  // Upload data
  
  // Quad data
  glBindBuffer(GL_ARRAY_BUFFER, g_quad_vbo);
  glEnableVertexAttribArray(SPRITE_ATTRIB_POSITION);
  glVertexAttribPointer(SPRITE_ATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), NULL);

  // Per instance data
  glBindBuffer(GL_ARRAY_BUFFER, sprite_batch->sprite_vbo);
  
  glEnableVertexAttribArray(SPRITE_ATTRIB_COLOR);
  glVertexAttribPointer(SPRITE_ATTRIB_COLOR, 4, GL_FLOAT, GL_FALSE, sizeof(Sprite), (void*) offsetof(Sprite, color));
  glVertexAttribDivisor(SPRITE_ATTRIB_COLOR, 1);

  glEnableVertexAttribArray(SPRITE_ATTRIB_SPRITE_UV);
  glVertexAttribPointer(SPRITE_ATTRIB_SPRITE_UV, 4, GL_FLOAT, GL_FALSE, sizeof(Sprite), (void*) offsetof(Sprite, sprite_uv));
  glVertexAttribDivisor(SPRITE_ATTRIB_SPRITE_UV, 1);

  // Model vbo
  glBindBuffer(GL_ARRAY_BUFFER, sprite_batch->model_vbo);  
  for (int i = 0; i < 4; i++)
  {
    int loc = SPRITE_ATTRIB_MODEL + i;
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*) (i * sizeof(glm::vec4)));
    glVertexAttribDivisor(loc, 1);
  }

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
};

void destroy_sprite_batch(SpriteBatch* sprite_batch)
{
  glDeleteVertexArrays(1, &sprite_batch->quad_vao);
  glDeleteBuffers(1, &sprite_batch->sprite_vbo);
  glDeleteBuffers(1, &sprite_batch->model_vbo);
}

void upload_sprite_batch_data(SpriteBatch* sprite_batch, Sprite* sprites, glm::mat4* models, size_t num_sprites)
{
  sprite_batch->num_sprites = num_sprites;
  
  //upload sprites to opengl
  glBindBuffer(GL_ARRAY_BUFFER, sprite_batch->sprite_vbo);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Sprite) * num_sprites, sprites);

  //upload transforms to opengl
  glBindBuffer(GL_ARRAY_BUFFER, sprite_batch->model_vbo);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::mat4) * num_sprites, models);
}

void render_sprites(SpriteBatch* sprite_batch, const Texture sprite_atlas, ShaderProgramID shader_program)
{  
  glBindVertexArray(sprite_batch->quad_vao);
  glUseProgram(shader_program);
  
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, sprite_atlas.texture_id);

  //draw instanced
  glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0, sprite_batch->num_sprites);
  
  glBindVertexArray(0);
}
