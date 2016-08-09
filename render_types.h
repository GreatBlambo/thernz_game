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
};

void create_sprite(Sprite* sprite, glm::vec2 texture_offset, glm::vec2 sprite_dims, Texture* texture);
bool sprite_is_valid(Sprite* sprite);
void color_sprite(Sprite* sprite, glm::vec4 color);
