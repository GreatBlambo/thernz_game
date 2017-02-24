#pragma once
#include <glm/glm.hpp>
#include <thirdparty/bitsquid-foundation-git/array.h>

#include "tzerror_codes.h"
#include "tzmemory.h"
#include "tzconfig.h"

namespace tz
{
  
TZ_HANDLE(ResourceHandle, uint32_t, 21);
struct ShaderHandle        { ResourceHandle id; };
struct ProgramHandle       { ResourceHandle id; };
struct TextureHandle       { ResourceHandle id; };
struct BindingHandle       { ResourceHandle id; };
struct BufferHandle        { ResourceHandle id; };
 
typedef glm::vec3 Color;

enum BufferType
{
  VERTEX_BUFFER,
  INDEX_BUFFER
};
 
enum BufferUsage
{
  STREAM_DRAW,
  STREAM_READ,
  STREAM_COPY,
  STATIC_DRAW,
  STATIC_READ,
  STATIC_COPY,
  DYNAMIC_DRAW,
  DYNAMIC_READ,
  DYNAMIC_COPY
};

enum DrawType
{
  POINTS,                       
  LINES,                          
  LINE_LOOP,
  LINE_STRIP,
  TRIANGLES,
  TRIANGLE_STRIP,                 
  TRIANGLE_FAN
};

enum TextureType
{
  TEXTURE_1D,
  TEXTURE_2D,
  TEXTURE_3D,
  TEXTURE_1D_ARRAY,
  TEXTURE_2D_ARRAY,
  TEXTURE_RECTANGLE,
  TEXTURE_CUBE_MAP,
  TEXTURE_CUBE_MAP_ARRAY,
  TEXTURE_BUFFER,
  TEXTURE_2D_MULTISAMPLE,
  TEXTURE_2D_MULTISAMPLE_ARRAY
};

enum ShaderType
{
  VERTEX_SHADER,
  TESS_CONTROL_SHADER,
  TESS_EVALUATION_SHADER,
  GEOMETRY_SHADER,
  FRAGMENT_SHADER,
  COMPUTE_SHADER
};

enum DataType
{
  UNSIGNED_BYTE,
  UNSIGNED_SHORT,
  UNSIGNED_INT,
  FLOAT  
};

enum BufferBits
{
  COLOR_BUFFER_BIT = 1,
  DEPTH_BUFFER_BIT = 1 << 1,
  ACCUM_BUFFER_BIT = 1 << 2,
  STENCIL_BUFFER_BIT = 1 << 3
};

typedef uint32_t GraphicsBitfield;

size_t get_type_size(DataType data_type);

#define TZ_PI 3.1415926535
#define TZ_DEGREES_TO_RADS(x) x * PI/180

////////////////////////////////////////////////////////////////////////////////
// Vertex Specification
////////////////////////////////////////////////////////////////////////////////

struct VertexAttribute
{
  char name[128];
  int location;
  int size;
  DataType type;

  size_t offset;
};

struct VertexLayout
{
  VertexLayout(const VertexAttribute* in_attribs, size_t in_num_attribs, foundation::Allocator& allocator);
  VertexLayout(const VertexLayout& other, foundation::Allocator& allocator);

  const VertexAttribute* attribs;
  size_t num_attribs;  
  size_t vert_size;

  const VertexAttribute& operator[](uint32_t i) const;
};
 
////////////////////////////////////////////////////////////////////////////////
// Texture
////////////////////////////////////////////////////////////////////////////////

struct Texture
{
  TextureHandle texture_id;
  TextureType type;
};

////////////////////////////////////////////////////////////////////////////////
// Material
////////////////////////////////////////////////////////////////////////////////

struct Material
{
  ProgramHandle shader;
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
}
