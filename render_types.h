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

////////////////////////////////////////////////////////////////////////////////
// Texture
////////////////////////////////////////////////////////////////////////////////

struct Texture
{
  TextureID texture_id;
  float w, h;
};

////////////////////////////////////////////////////////////////////////////////
// Sprite
////////////////////////////////////////////////////////////////////////////////

struct Sprite
{
  glm::vec4 color;
  glm::vec4 sprite_uv;
};

void create_sprite(Sprite* sprite, glm::vec2 texture_offset, glm::vec2 sprite_dims, Texture* texture, Color color = { 1.0f, 1.0f, 1.0f });
bool sprite_is_valid(Sprite* sprite);
void color_sprite(Sprite* sprite, glm::vec4 color);

////////////////////////////////////////////////////////////////////////////////
// Vertex Specification
////////////////////////////////////////////////////////////////////////////////

struct VertSpec
{
  const char** attrib_names;
  int* attrib_locations;
  size_t num_attributes;
};

