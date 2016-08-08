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

void create_sprite_batch(SpriteBatch* sprite_batch, float screen_w, float screen_h, ShaderProgramID shader_program)
{  
  sprite_batch->shader_program = shader_program;
  sprite_batch->model_loc = glGetUniformLocation(shader_program, "model");
  sprite_batch->projection_loc = glGetUniformLocation(shader_program, "projection");
  sprite_batch->sprite_color_loc = glGetUniformLocation(shader_program, "sprite_color");

  glGenVertexArrays(1, &sprite_batch->quad_vao);
  glGenBuffers(1, &sprite_batch->quad_vbo);
  glGenBuffers(1, &sprite_batch->quad_ibo);
  
  glBindVertexArray(sprite_batch->quad_vao);
  
  glBindBuffer(GL_ARRAY_BUFFER, sprite_batch->quad_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);
  
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sprite_batch->quad_ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quad_indices), quad_indices, GL_STATIC_DRAW);
  
  int vert_loc = glGetAttribLocation(shader_program, "vertex");
  glEnableVertexAttribArray(vert_loc);
  glVertexAttribPointer(vert_loc, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), NULL);

  sprite_batch->screen_w = screen_w;
  sprite_batch->screen_h = screen_h;

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
  glm::mat4 projection = glm::ortho(0.0f, sprite_batch->screen_w, sprite_batch->screen_h, 0.0f, -1.0f, 1.0f);
  
  glUseProgram(sprite_batch->shader_program);
  glBindVertexArray(sprite_batch->quad_vao);
  glActiveTexture(GL_TEXTURE0);
  
  TextureID current_texture = 0;
  for (size_t i = 0; i < num_sprites; i++)
  {
    if (current_texture != sprites[i].texture
	|| i == 0)
    {
      current_texture = sprites[i].texture;
      glBindTexture(GL_TEXTURE_2D, current_texture);
    }
    
    glUniformMatrix4fv(sprite_batch->model_loc, 1, false, glm::value_ptr(sprites[i].model));
    glUniformMatrix4fv(sprite_batch->projection_loc, 1, false, glm::value_ptr(projection));    
    glUniform3fv(sprite_batch->sprite_color_loc, 1, glm::value_ptr(sprites[i].color));
    
    glDrawElements(GL_TRIANGLES, ARRAY_SIZE(quad_indices), GL_UNSIGNED_SHORT, 0);
  }
  
  glBindVertexArray(0);
}
