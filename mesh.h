#include <glm/glm.hpp>

#include "render_types.h"

struct MeshData
{
  glm::vec3* positions;
  glm::vec3* normals;
  glm::vec2* tex_coords;
  size_t* indices;
  
  size_t num_verts;
};

struct Mesh
{
  Texture texture;
  glm::vec2 uv_offset;
  
  VertexBufferID vertex_buffer;
  VertexArrayID index_buffer;
  IndexArrayID attrib_array;
  size_t num_indices;
};

extern VertSpec g_mesh_vert_spec;

void create_mesh(Mesh* result, const MeshData* mesh_data);
void destroy_mesh(Mesh* mesh);
void draw_mesh(Mesh* m);
