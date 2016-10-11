#include <thirdparty/tinyobjloader/tiny_obj_loader.h>
#include <string>
#include "mesh.h"

VertSpec g_mesh_vert_spec; // TODO: Define this

bool load_mesh_from_obj(Mesh* result, const char* filename)
{
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  
  std::string err;
  bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filename);

  if (!err.empty())
  {
    fprintf(stderr, "%s\n", err.c_str());
    return false;
  }

  if (!ret)
  {
    fprintf(stderr, "OBJ load error when loading %s\n", filename);
    return false;
  }  

  return true;
}

void destroy_mesh(Mesh* mesh)
{
}

void draw_mesh(Mesh* m)
{
}

