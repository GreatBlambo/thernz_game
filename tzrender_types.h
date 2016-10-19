#pragma once
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <glm/glm.hpp>

#include "tzerror_codes.h"
#include "tzmemory.h"
#include "tzconfig.h"

namespace tz
{
// GL stuff
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

enum TextureType : GLenum
{
  TEXTURE_1D = GL_TEXTURE_1D,
  TEXTURE_2D = GL_TEXTURE_2D,
  TEXTURE_3D = GL_TEXTURE_3D,
  TEXTURE_1D_ARRAY = GL_TEXTURE_1D_ARRAY,
  TEXTURE_2D_ARRAY = GL_TEXTURE_2D_ARRAY,
  TEXTURE_RECTANGLE = GL_TEXTURE_RECTANGLE,
  TEXTURE_CUBE_MAP = GL_TEXTURE_CUBE_MAP,
  TEXTURE_CUBE_MAP_ARRAY = GL_TEXTURE_CUBE_MAP_ARRAY,
  TEXTURE_BUFFER = GL_TEXTURE_BUFFER,
  TEXTURE_2D_MULTISAMPLE = GL_TEXTURE_2D_MULTISAMPLE,
  TEXTURE_2D_MULTISAMPLE_ARRAY = GL_TEXTURE_2D_MULTISAMPLE_ARRAY
};

enum ShaderType : GLenum
{
  VERTEX_SHADER = GL_VERTEX_SHADER,
  TESS_CONTROL_SHADER =  GL_TESS_CONTROL_SHADER,
  TESS_EVALUATION_SHADER = GL_TESS_EVALUATION_SHADER,
  GEOMETRY_SHADER = GL_GEOMETRY_SHADER,
  FRAGMENT_SHADER = GL_FRAGMENT_SHADER,
  COMPUTE_SHADER = GL_COMPUTE_SHADER
};

enum DataType : GLenum
{
  UNSIGNED_BYTE = GL_UNSIGNED_BYTE,
  UNSIGNED_SHORT = GL_UNSIGNED_SHORT,
  UNSIGNED_INT = GL_UNSIGNED_INT
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
  TextureType type;
};

////////////////////////////////////////////////////////////////////////////////
// Material
////////////////////////////////////////////////////////////////////////////////

struct Material
{
  ShaderProgramID shader;
  Texture textures[TZ_CONFIG_MAX_NUM_SAMPLERS];
  size_t num_textures;
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
GameError load_image_as_texture(int* width, int* height, Texture* texture, const char* pathname);
void destroy_texture(Texture* texture);

// Shaders
ShaderID load_shader_source(const char* pathname, ShaderType shader_type);
void destroy_shader(ShaderID shader);
ShaderProgramID link_shader_program(ShaderID* shaders, size_t num_shaders, const VertSpec& vertex_spec);
void destroy_program(ShaderProgramID program);
GameError detach_shaders(ShaderProgramID program, ShaderID* shaders, size_t num_shaders);

}
