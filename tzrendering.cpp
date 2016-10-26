#include "tzrendering.h"
#include "tzhash.h"
#include "tzsort.h"

#include "tzbackend_dispatch.h"
#include "tzbackend_types.h"

namespace tz
{

namespace renderer
{

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
#define SORT_CMD_FLG_MASK      (TZ_BIT_MASK(SORT_CMD_FLG_BITS         ) << SORT_CMD_FLG_SHIFT)
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
                 + SORT_VIEW_BITS);
TZ_STATIC_ASSERT(64 ==
  SORT_COMMAND_BITS
                 + SORT_CMD_FLG_BITS
                 + SORT_TRANSLUCENCY_BITS
                 + SORT_PASS_BITS
                 + SORT_VIEW_BITS);
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
                  ^ SORT_VIEW_MASK));
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
                  ^ SORT_VIEW_MASK));

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

IBackend* g_backend;
GLBackend g_backend_gl;

void init_rendering()
{
  // init default renderer and backend
  g_backend = &g_backend_gl;
}

void deinit_rendering()
{
  // destroy default renderer
}

void push_frame()
{
  // submit rendering thru default renderer
}

}
}
