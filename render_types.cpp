#include <glm/gtc/matrix_transform.hpp>
#include "render_types.h"

void create_sprite(Sprite* sprite, glm::vec2 texture_offset, glm::vec2 sprite_dims, Texture* texture)
{
  sprite->sprite_uv.x = texture_offset.x / texture->w;
  sprite->sprite_uv.y = texture_offset.y / texture->h;
  sprite->sprite_uv.w = sprite_dims.x / texture->w;
  sprite->sprite_uv.z = sprite_dims.y / texture->h;
}

void sprite_apply_transform(Sprite* sprite, glm::mat4 transform_matrix)
{
  sprite->model *= transform_matrix;
}

void transform_sprite(Sprite* sprite, glm::vec3 position, glm::vec2 size,
		      float rotate, glm::vec3 rotation_axis)
{
  glm::mat4 mvm;
  mvm = glm::translate(mvm, position);
  mvm = glm::rotate(mvm, rotate, rotation_axis);
  mvm = glm::translate(mvm, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));
  mvm = glm::scale(mvm, glm::vec3(size, 1.0f));

  sprite->model = mvm;
}
void sprite_set_position(Sprite* sprite, glm::vec3 position)
{
  sprite->model = glm::translate(sprite->model, position);
}
void sprite_set_rotation(Sprite* sprite, float rotate, glm::vec3 rotation_axis)
{
  sprite->model = glm::rotate(sprite->model, rotate, rotation_axis);
}

void color_sprite(Sprite* sprite, glm::vec4 color)
{
  sprite->color = color;
}
