#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "sprite_batch.h"
#include "render_types.h"

////////////////////////////////////////////////////////////////////////////////
// Sprite batching
////////////////////////////////////////////////////////////////////////////////

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

#define SPRITE_ATTRIB_POSITION 0
#define SPRITE_ATTRIB_COLOR 1
#define SPRITE_ATTRIB_SPRITE_UV 2
#define SPRITE_ATTRIB_MODEL 3

static const char* sprite_attrib_names[] = {
  "position",
  "color",
  "sprite_uv",
  "model"
};

static int sprite_attrib_locations[] = {
  SPRITE_ATTRIB_POSITION,
  SPRITE_ATTRIB_COLOR,
  SPRITE_ATTRIB_SPRITE_UV,
  SPRITE_ATTRIB_MODEL
};

VertSpec g_sprite_batch_vert_spec = {
  .attrib_names = sprite_attrib_names,
  .attrib_locations = sprite_attrib_locations,
  .num_attributes = ARRAY_SIZE(sprite_attrib_names)
};

void create_sprite_batch(SpriteBatch* sprite_batch,
                         float screen_w, float screen_h,
                         const Texture sprite_atlas, ShaderProgramID shader_program,
                         size_t max_sprites)
{
  
  // Get uniform locations
  sprite_batch->shader_program = shader_program;
  sprite_batch->projection_loc = glGetUniformLocation(shader_program, "projection");
  sprite_batch->view_loc = glGetUniformLocation(shader_program, "view");

  // Set parameters
  sprite_batch->screen_w = screen_w;
  sprite_batch->screen_h = screen_h;
  sprite_batch->sprite_atlas = sprite_atlas;
  sprite_batch->num_sprites = 0;
  
  // Initialize buffers
  glGenVertexArrays(1, &sprite_batch->quad_vao);
  glGenBuffers(1, &sprite_batch->quad_vbo);
  glGenBuffers(1, &sprite_batch->quad_ibo);
  glGenBuffers(1, &sprite_batch->sprite_vbo);
  glGenBuffers(1, &sprite_batch->model_vbo);

  // Allocate vbos
  glBindBuffer(GL_ARRAY_BUFFER, sprite_batch->sprite_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Sprite) * max_sprites, NULL, GL_DYNAMIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, sprite_batch->model_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * max_sprites, NULL, GL_DYNAMIC_DRAW);

  // Upload sprite quad
  glBindBuffer(GL_ARRAY_BUFFER, sprite_batch->quad_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sprite_batch->quad_ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quad_indices), quad_indices, GL_STATIC_DRAW);
  
  // Set vertex attributes
  glBindVertexArray(sprite_batch->quad_vao);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sprite_batch->quad_ibo);
  
  // Upload data
  
  // Quad data
  glBindBuffer(GL_ARRAY_BUFFER, sprite_batch->quad_vbo);
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
  glDeleteBuffers(1, &sprite_batch->quad_vbo);
  glDeleteBuffers(1, &sprite_batch->quad_ibo);
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

void render_sprites(SpriteBatch* sprite_batch, glm::mat4 view)
{
  //static glm::mat4 projection = glm::ortho(0.0f, 1.0f, 0.0f, 1.0f, -1.0f, 1.0f);
  static glm::mat4 projection = glm::ortho(0.0f, sprite_batch->screen_w, sprite_batch->screen_h, 0.0f, -1.0f, 1.0f);
  
  glBindVertexArray(sprite_batch->quad_vao);
  glUseProgram(sprite_batch->shader_program);
  glUniformMatrix4fv(sprite_batch->projection_loc, 1, false, glm::value_ptr(projection));
  glUniformMatrix4fv(sprite_batch->view_loc, 1, false, glm::value_ptr(view));
  
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, sprite_batch->sprite_atlas.texture_id);

  //draw instanced
  glDrawElementsInstanced(GL_TRIANGLES, ARRAY_SIZE(quad_indices), GL_UNSIGNED_SHORT, 0, sprite_batch->num_sprites);
  
  glBindVertexArray(0);
}
