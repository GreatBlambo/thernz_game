#include "tzrendering.h"
#include "tzhash.h"
#include "tzsort.h"

#include "tzbackend_dispatch.h"
#include "tzbackend_types.h"

namespace tz
{

namespace renderer
{

typedef uint32_t MaterialID;
typedef uint64_t Key;

////////////////////////////////////////////////////////////////////////////////
// Renderer
////////////////////////////////////////////////////////////////////////////////

/** Key Bitfield
 * Draw call
 * | View (8)
 * | Pass (5)
 * | Translucency (2)
 * | CMD_FLG (1)
 * | Material ID (16)
 * + | Shader Program Hash (8)
 * + | Texture Hash (8)
 * | Depth (32)
 *
 * Command
 * | View (8)
 * | Pass (5)
 * | Translucency (2)
 * | CMD_FLG (1)
 * | Command Bits (48)
 * 
 * Note: for batched or instanced objects, you are responsible for depth sorting.
 */

#define SORT_DEPTH_BITS        32
#define SORT_TEXTURE_BITS      8
#define SORT_SHADER_BITS       8
#define SORT_MATERIAL_BITS     SORT_SHADER_BITS + SORT_TEXTURE_BITS
#define SORT_CMD_FLG_BITS      1
#define SORT_TRANSLUCENCY_BITS 2
#define SORT_PASS_BITS         5
#define SORT_VIEW_BITS         8

#define SORT_DEPTH_SHIFT        0
#define SORT_TEXTURE_SHIFT      SORT_DEPTH_BITS
#define SORT_SHADER_SHIFT       SORT_TEXTURE_SHIFT + SORT_TEXTURE_BITS
#define SORT_MATERIAL_SHIFT     SORT_DEPTH_BITS
#define SORT_CMD_FLG_SHIFT      SORT_MATERIAL_SHIFT + SORT_MATERIAL_BITS
#define SORT_TRANSLUCENCY_SHIFT SORT_CMD_FLG_SHIFT + SORT_CMD_FLG_BITS
#define SORT_PASS_SHIFT         SORT_TRANSLUCENCY_SHIFT + SORT_TRANSLUCENCY_BITS
#define SORT_VIEW_SHIFT         SORT_PASS_SHIFT + SORT_PASS_BITS

#define SORT_DEPTH_MASK         TZ_BIT_MASK(SORT_DEPTH_BITS)
#define SORT_TEXTURE_MASK      (TZ_BIT_MASK(SORT_TEXTURE_BITS     ) << SORT_TEXTURE_SHIFT)
#define SORT_SHADER_MASK       (TZ_BIT_MASK(SORT_SHADER_BITS      ) << SORT_SHADER_SHIFT)
#define SORT_MATERIAL_MASK     (TZ_BIT_MASK(SORT_MATERIAL_BITS    ) << SORT_MATERIAL_SHIFT)
#define SORT_CMD_FLG_MASK      (TZ_BIT_MASK(SORT_CMD_FLG_BITS     ) << SORT_CMD_FLG_SHIFT)
#define SORT_TRANSLUCENCY_MASK (TZ_BIT_MASK(SORT_TRANSLUCENCY_BITS) << SORT_TRANSLUCENCY_SHIFT)
#define SORT_PASS_MASK         (TZ_BIT_MASK(SORT_PASS_BITS        ) << SORT_PASS_SHIFT)
#define SORT_VIEW_MASK         (TZ_BIT_MASK(SORT_VIEW_BITS        ) << SORT_VIEW_SHIFT)

#define SORT_COMMAND_BITS  48
#define SORT_COMMAND_SHIFT 0
#define SORT_COMMAND_MASK  (TZ_BIT_MASK(SORT_COMMAND_BITS) << SORT_COMMAND_SHIFT)

// Bits add up to 64
TZ_STATIC_ASSERT(64 ==
                 SORT_DEPTH_BITS
                 + SORT_MATERIAL_BITS
                 + SORT_CMD_FLG_BITS
                 + SORT_TRANSLUCENCY_BITS
                 + SORT_PASS_BITS
                 + SORT_VIEW_BITS, wrong_number_bits_draw);
TZ_STATIC_ASSERT(64 ==
                 SORT_COMMAND_BITS
                 + SORT_CMD_FLG_BITS
                 + SORT_TRANSLUCENCY_BITS
                 + SORT_PASS_BITS
                 + SORT_VIEW_BITS, wrong_number_bits_command);
// Bit fields don't overlap
TZ_STATIC_ASSERT((0 | SORT_DEPTH_MASK
                  | SORT_MATERIAL_MASK
                  | SORT_CMD_FLG_MASK
                  | SORT_TRANSLUCENCY_MASK
                  | SORT_PASS_MASK
                  | SORT_VIEW_MASK)
                 ==
                 (0 ^ SORT_DEPTH_MASK
                  ^ SORT_MATERIAL_MASK
                  ^ SORT_CMD_FLG_MASK
                  ^ SORT_TRANSLUCENCY_MASK
                  ^ SORT_PASS_MASK
                  ^ SORT_VIEW_MASK), overlap_draw);
TZ_STATIC_ASSERT((0 | SORT_COMMAND_MASK
                  | SORT_CMD_FLG_MASK
                  | SORT_TRANSLUCENCY_MASK
                  | SORT_PASS_MASK
                  | SORT_VIEW_MASK)
                 ==
                 (0 ^ SORT_COMMAND_MASK
                  ^ SORT_CMD_FLG_MASK
                  ^ SORT_TRANSLUCENCY_MASK
                  ^ SORT_PASS_MASK
                  ^ SORT_VIEW_MASK), overlap_command);

MaterialID create_material_id(ShaderProgramID program, TextureID* textures, size_t num_textures)
{
TZ_ASSERT(num_textures < TZ_CONFIG_MAX_NUM_SAMPLERS, "Too many textures!");
  
const uint32_t texture_hash =  hash::fnv1a((byte*) textures, num_textures           ) & TZ_BIT_MASK(SORT_TEXTURE_BITS);
const uint32_t shader_hash  = (hash::fnv1a((byte*) &program, sizeof(ShaderProgramID)) & TZ_BIT_MASK(SORT_SHADER_BITS)) << SORT_TEXTURE_BITS;
  
return shader_hash | texture_hash;
}

struct KeyData
{
  float m_depth;
  uint32_t m_material_id;
  uint8_t m_trans;
  uint8_t m_pass;
  uint8_t m_view;

  Key encode_key()
  {    
    const uint64_t depth        = (uint64_t(m_depth)       << SORT_DEPTH_SHIFT       ) & SORT_DEPTH_MASK;
    const uint64_t material_id  = (uint64_t(m_material_id) << SORT_MATERIAL_SHIFT    ) & SORT_MATERIAL_MASK;
    const uint64_t translucency = (uint64_t(m_trans)       << SORT_TRANSLUCENCY_SHIFT) & SORT_PASS_MASK;
    const uint64_t pass         = (uint64_t(m_pass)        << SORT_PASS_SHIFT        ) & SORT_PASS_MASK;
    const uint64_t view         = (uint64_t(m_view)        << SORT_VIEW_SHIFT        ) & SORT_VIEW_MASK;

    return view | pass | translucency | material_id | depth;
  }

  void decode_key(Key key)
  {
    m_depth       = (key & SORT_DEPTH_MASK        ) >> SORT_DEPTH_SHIFT;
    m_material_id = (key & SORT_MATERIAL_MASK     ) >> SORT_MATERIAL_SHIFT;
    m_trans       = (key & SORT_TRANSLUCENCY_SHIFT) >> SORT_TRANSLUCENCY_SHIFT;
    m_pass        = (key & SORT_PASS_SHIFT        ) >> SORT_PASS_SHIFT;
    m_view        = (key & SORT_VIEW_SHIFT        ) >> SORT_VIEW_SHIFT;
  }
};

////////////////////////////////////////////////////////////////////////////////
// Module interface
////////////////////////////////////////////////////////////////////////////////

// Any code inside these functions is temporary until we develop a default renderer

static IBackend* g_backend;
static BackendGL g_backend_gl;

void init_rendering()
{
  // init default renderer and backend
  set_backend(TZ_CONFIG_DEFAULT_GFX_BACKEND);
  fatal_game_error(g_backend->init());
}

void deinit_rendering()
{
  // destroy default renderer
  g_backend->deinit();
}

void push_frame()
{
  // submit rendering thru default renderer
  swap_backbuffer();
}

void set_backend(BackendType type)
{
  switch (type)
  {
   case OPENGL:
     g_backend = &g_backend_gl;
     BackendGL::bind_commands();
     break;
  }
}

void set_window_name(const char* name)
{
  g_backend->set_window_name(name);
}

void set_window_size(int width, int height)
{
  g_backend->set_window_size(width, height);
}

void set_window_fullscreen()
{
  g_backend->set_window_fullscreen();
}

void set_window_fullscreen_windowed()
{
  g_backend->set_window_fullscreen_windowed();
}

void set_window_windowed()
{
  g_backend->set_window_windowed();
}

void set_window_position(int x, int y)
{
  g_backend->set_window_position(x, y);
}

void swap_backbuffer()
{
  g_backend->swap_backbuffer();
}

void clear_backbuffer(GraphicsBitfield bitfield)
{
  g_backend->clear_backbuffer(bitfield);
}

GameError load_image_as_texture(int* width, int* height, Texture* texture, const char* pathname)
{
  g_backend->load_image_as_texture(width, height, texture, pathname);
}

void destroy_texture(Texture* texture)
{
  g_backend->destroy_texture(texture);
}

ShaderID load_shader_source(const char* pathname, ShaderType shader_type)
{
  g_backend->load_shader_source(pathname, shader_type);
}

void destroy_shader(ShaderID shader)
{
  g_backend->destroy_shader(shader);
}

ShaderProgramID link_shader_program(ShaderID* shaders, size_t num_shaders, const VertexAttribArray& vertex_spec)
{
  g_backend->link_shader_program(shaders, num_shaders, vertex_spec);
}

void destroy_program(ShaderProgramID program)
{
  g_backend->destroy_program(program);
}

GameError detach_shaders(ShaderProgramID program, ShaderID* shaders, size_t num_shaders)
{
  g_backend->detach_shaders(program, shaders, num_shaders);
}


}
}
