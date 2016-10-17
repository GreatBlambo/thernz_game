#pragma once
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <glm/glm.hpp>

#include "tzerror_codes.h"
#include "tzmemory.h"

namespace tz
{

typedef glm::vec3 Color;

typedef GLuint TextureID;
typedef GLuint VertexBufferID;
typedef GLuint VertexArrayID;
typedef GLuint IndexArrayID;
typedef GLuint UniformBufferID;

typedef GLuint FrameBufferID;

typedef GLuint ShaderID;
typedef GLuint ShaderProgramID;

enum DrawType : GLenum
{
  POINTS = GL_POINTS,                       
  LINES = GL_LINES,                          
  LINE_LOOP = GL_LINE_LOOP,
  LINE_STRIP = GL_LINE_STRIP,
  TRIANGLES = GL_TRIANGLES,
  TRIANGLE_STRIP = GL_TRIANGLE_STRIP,                 
  TRIANGLE_FAN = GL_TRIANGLE_FAN
};


#define TZ_PI 3.1415926535
#define TZ_DEGREES_TO_RADS(x) x * PI/180

////////////////////////////////////////////////////////////////////////////////
// Vertex Specification
////////////////////////////////////////////////////////////////////////////////

struct VertSpec
{
  const char** attrib_names;
  int* attrib_locations;
  size_t num_attributes;
};

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

////////////////////////////////////////////////////////////////////////////////
// Assets
////////////////////////////////////////////////////////////////////////////////

// Textures
GameError load_image_as_texture(Texture* texture, const char* pathname);
bool texture_is_valid(Texture* texture);
void destroy_texture(Texture* texture);

// Shaders
ShaderID load_shader_source(const char* pathname, GLenum shader_type);
void destroy_shader(ShaderID shader);
ShaderProgramID link_shader_program(ShaderID* shaders, size_t num_shaders, const VertSpec& vertex_spec);
void destroy_program(ShaderProgramID program);
GameError detach_shaders(ShaderProgramID program, ShaderID* shaders, size_t num_shaders);

}
