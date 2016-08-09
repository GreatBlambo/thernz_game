#pragma once
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <glm/glm.hpp>

typedef glm::vec3 Color;

typedef GLuint TextureID;
typedef GLuint VertexBufferID;
typedef GLuint VertexArrayID;
typedef GLuint IndexArrayID;

typedef GLuint ShaderID;
typedef GLuint ShaderProgramID;

#define PI 3.1415926535
#define DEGREES_TO_RADS(x) x * PI/180

struct Texture
{
  TextureID texture_id;
  float w, h;
};

struct Sprite
{
  glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
  glm::vec4 sprite_uv;
  glm::mat4 model;
};

void create_sprite(Sprite* sprite, glm::vec2 texture_offset, glm::vec2 sprite_dims, Texture* texture);
bool sprite_is_valid(Sprite* sprite);
void sprite_apply_transform(Sprite* sprite, glm::mat4 transform_matrix);
void transform_sprite(Sprite* sprite, glm::vec3 position, glm::vec2 size,
		      float rotate, glm::vec3 rotation_axis);
void sprite_translate(Sprite* sprite, glm::vec3 transvec);
void sprite_rotate(Sprite* sprite, float rotate, glm::vec3 rotation_axis);
void color_sprite(Sprite* sprite, glm::vec4 color);
