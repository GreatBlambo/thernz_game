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

VertexLayout::VertexLayout(const VertexAttribute* in_attribs, size_t in_num_attribs, foundation::Allocator& allocator)
  : num_attribs(in_num_attribs)
  , vert_size(0)
{
  attribs = allocator.allocate(num_attribs * sizeof(VertexAttribute));
  memcpy(attribs, in_attribs, num_attribs * sizeof(VertexAttribute));
  for (int i = 0; i < num_attribs; i++)
  {
    vert_size += attribs[i].offset + attribs[i].size;
  }
}

VertexLayout(const VertexLayout& other, foundation::Allocator& allocator)
  : num_attribs(other.num_attribs)
  , vert_size(other.vert_size)
{
  
  attribs = allocator.allocate(num_attribs * sizeof(VertexAttribute));
  memcpy(attribs, other.attribs, num_attribs * sizeof(VertexAttribute));
}

const VertexAttribute& VertexLayout::operator[](uint32_t i) const
{
  return attribs[i];
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
