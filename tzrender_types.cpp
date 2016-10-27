#include <glm/gtc/matrix_transform.hpp>
#include <SDL2/SDL_image.h>

#include "tzrender_types.h"

namespace tz
{

////////////////////////////////////////////////////////////////////////////////
// Types
////////////////////////////////////////////////////////////////////////////////

size_t get_type_size(DataType data_type)
{
  switch(data_type)
  {
   case(UNSIGNED_BYTE):
     return sizeof(byte);
     break;
   case(UNSIGNED_SHORT):
     return sizeof(uint16_t);
     break;
   case(UNSIGNED_INT):
     return sizeof(uint32_t);
     break;
   case(FLOAT):
     return sizeof(float);
     break;
  }
}

////////////////////////////////////////////////////////////////////////////////
// Vertex Specification
////////////////////////////////////////////////////////////////////////////////

VertexAttribArray::VertexAttribArray(foundation::Allocator& alloc)
  : foundation::Array<VertexAttribute>(alloc)
{}

void push_attrib(VertexAttribArray& array, const char* name, int location, int size, DataType type)
{
  VertexAttribute new_attrib;
  strcpy(new_attrib.name, name);
  new_attrib.location = location;
  new_attrib.size = size;
  new_attrib.type = type;

  if (foundation::array::size(array) == 0)
  {
    new_attrib.offset = 0;
  }
  else
  {
    VertexAttribute& last = foundation::array::back(array);
    new_attrib.offset = last.offset + (last.size * get_type_size(last.type));
  }

  array.vert_size = new_attrib.offset + (size * get_type_size(type));

  foundation::array::push_back(array, new_attrib);
}

////////////////////////////////////////////////////////////////////////////////
// Sprites
////////////////////////////////////////////////////////////////////////////////

void create_sprite(Sprite* sprite, glm::vec2 texture_offset, glm::vec2 tex_dims, glm::vec2 sprite_dims, Texture* texture, Color color)
{
  sprite->sprite_uv.x = texture_offset.x / tex_dims.x;
  sprite->sprite_uv.y = texture_offset.y / tex_dims.y;
  sprite->sprite_uv.w = sprite_dims.x / tex_dims.x;
  sprite->sprite_uv.z = sprite_dims.y / tex_dims.y;
  sprite->color = glm::vec4(color, 1.0);
}

}
