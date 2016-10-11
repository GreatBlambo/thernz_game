#pragma once

#include <glm/glm.hpp>

#include "render_types.h"
#include "memory.h"

////////////////////////////////////////////////////////////////////////////////
// Mesh (render data for meshes)
////////////////////////////////////////////////////////////////////////////////

struct Mesh
{
  Texture texture;
  glm::vec2 uv_offset;
  
  VertexBufferID vertex_buffer;
  VertexArrayID index_buffer;
  IndexArrayID attrib_array;
  size_t num_indices;
};

struct Material
{
};

extern VertSpec g_mesh_vert_spec;

bool load_mesh_from_obj(Mesh* result, const char* filename);
void destroy_mesh(Mesh* mesh);
void draw_mesh(Mesh* m);
