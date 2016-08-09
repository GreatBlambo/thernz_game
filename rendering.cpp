#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "rendering.h"

static const GLfloat quad_vertices[] = {
  1.0f,  1.0f, // Top Right
  1.0f,  0.0f, // Bottom Right
  0.0f,  0.0f, // Bottom Left
  0.0f,  1.0f  // Top Left
};

static const uint16_t quad_indices[] = {
  0, 1, 3,
  1, 2, 3
};

void create_sprite_batch(SpriteBatch* sprite_batch,
                         float screen_w, float screen_h,
                         Texture sprite_atlas, ShaderProgramID shader_program)
{ 
  // Get uniform locations
  sprite_batch->shader_program = shader_program;
  sprite_batch->projection_loc = glGetUniformLocation(shader_program, "projection");
  sprite_batch->view_loc = glGetUniformLocation(shader_program, "view");

  // Set parameters
  sprite_batch->screen_w = screen_w;
  sprite_batch->screen_h = screen_h;
  sprite_batch->sprite_atlas = sprite_atlas;
  
  // Initialize buffers

  glGenVertexArrays(1, &sprite_batch->quad_vao);
  glGenBuffers(1, &sprite_batch->quad_vbo);
  glGenBuffers(1, &sprite_batch->quad_ibo);
  glGenBuffers(1, &sprite_batch->sprite_vbo);
  
  glBindVertexArray(sprite_batch->quad_vao);
  
  glBindBuffer(GL_ARRAY_BUFFER, sprite_batch->quad_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);
  
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sprite_batch->quad_ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quad_indices), quad_indices, GL_STATIC_DRAW);

  // Set vertex attributes
  // Quad data
  int pos_loc = glGetAttribLocation(shader_program, "position");
  glEnableVertexAttribArray(pos_loc);
  glVertexAttribPointer(pos_loc, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), NULL);

  // Per instance data
  glBindBuffer(GL_ARRAY_BUFFER, sprite_batch->sprite_vbo);
  
  int color_loc = glGetAttribLocation(shader_program, "color");
  glEnableVertexAttribArray(color_loc);
  glVertexAttribPointer(color_loc, 4, GL_FLOAT, GL_FALSE, sizeof(Sprite), (void*) offsetof(Sprite, color));
  glVertexAttribDivisor(color_loc, 1);
  
  int sprite_uv_loc = glGetAttribLocation(shader_program, "sprite_uv");
  glEnableVertexAttribArray(sprite_uv_loc);
  glVertexAttribPointer(sprite_uv_loc, 4, GL_FLOAT, GL_FALSE, sizeof(Sprite), (void*) offsetof(Sprite, sprite_uv));
  glVertexAttribDivisor(sprite_uv_loc, 1);
  
  int model_loc = glGetAttribLocation(shader_program, "model");
  for (int i = 0; i < 4; i++)
  {
    int loc = model_loc + i;
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, sizeof(Sprite), (void*) (offsetof(Sprite, model) + (i * sizeof(glm::vec4))));
    glVertexAttribDivisor(loc, 1);
  }
  
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
};

void destroy_sprite_batch(SpriteBatch* sprite_batch)
{
  glDeleteVertexArrays(1, &sprite_batch->quad_vao);
  glDeleteBuffers(1, &sprite_batch->quad_vbo);
  glDeleteBuffers(1, &sprite_batch->quad_ibo);
}

void render_sprites(SpriteBatch* sprite_batch, Sprite* sprites, size_t num_sprites)
{
  static glm::mat4 projection = glm::ortho(0.0f, sprite_batch->screen_w, sprite_batch->screen_h, 0.0f, -1.0f, 1.0f);
  static glm::mat4 view(1.0f); //temp

  //upload sprites to opengl
  glBindBuffer(GL_ARRAY_BUFFER, sprite_batch->sprite_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Sprite) * num_sprites, sprites, GL_DYNAMIC_DRAW);
  
  glBindVertexArray(sprite_batch->quad_vao);
  glUseProgram(sprite_batch->shader_program);
  glUniformMatrix4fv(sprite_batch->projection_loc, 1, false, glm::value_ptr(projection));
  glUniformMatrix4fv(sprite_batch->view_loc, 1, false, glm::value_ptr(view));
  
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, sprite_batch->sprite_atlas.texture_id);

  //draw instanced
  glDrawElementsInstanced(GL_TRIANGLES, ARRAY_SIZE(quad_indices), GL_UNSIGNED_SHORT, 0, num_sprites);
  
  glBindVertexArray(0);
}
