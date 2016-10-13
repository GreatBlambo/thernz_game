#include "render_globals.h"
#include "memory.h"

void init_sprites();
void deinit_sprites();

void init_rendering()
{
  init_sprites();
}

void deinit_rendering()
{
  deinit_sprites();
}

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

VertexBufferID g_quad_vbo;
IndexArrayID g_quad_ibo;

void init_sprites()
{
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
  
  { // Initialize buffers
    glGenBuffers(1, &g_quad_vbo);
    glGenBuffers(1, &g_quad_ibo);
  
    // Upload sprite quad
    glBindBuffer(GL_ARRAY_BUFFER, g_quad_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_quad_ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quad_indices), quad_indices, GL_STATIC_DRAW);
  }
}

void deinit_sprites()
{
  glDeleteBuffers(1, &g_quad_vbo);
  glDeleteBuffers(1, &g_quad_ibo);  
}


