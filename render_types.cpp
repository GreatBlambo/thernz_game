#include <glm/gtc/matrix_transform.hpp>
#include "render_types.h"

void create_sprite(Sprite* sprite, glm::vec2 texture_offset, glm::vec2 sprite_dims, Texture* texture)
{
  sprite->sprite_uv.x = texture_offset.x / texture->w;
  sprite->sprite_uv.y = texture_offset.y / texture->h;
  sprite->sprite_uv.w = sprite_dims.x / texture->w;
  sprite->sprite_uv.z = sprite_dims.y / texture->h;
}
