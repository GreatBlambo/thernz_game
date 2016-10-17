#include "tzrendering.h"
#include "tzhash.h"
#include "tzsort.h"

namespace tz
{

namespace renderer
{
////////////////////////////////////////////////////////////////////////////////
// CommandBuffer
////////////////////////////////////////////////////////////////////////////////

// TODO!

////////////////////////////////////////////////////////////////////////////////
// Renderer
////////////////////////////////////////////////////////////////////////////////

/** Key Bitfield
 * Draw call
 * | View (8) | Pass (5) | Translucency (2) | CMD_FLG (1) |               Material ID (16)                | Depth (32) |
 *                                                        ^                                               ^
 *                                                        |                                               |
 *                                                        + | Shader Program Hash (8) | Texture Hash (8) |+
 *
 * Command
 * | View (8) | Pass (5) | Translucency (2) | CMD_FLG (1) | Command Bits (48) |
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

#define SORT_DEPTH_MASK         BIT_MASK(SORT_DEPTH_BITS)
#define SORT_TEXTURE_MASK      (BIT_MASK(SORT_TEXTURE_BITS     ) << SORT_TEXTURE_SHIFT)
#define SORT_SHADER_MASK       (BIT_MASK(SORT_SHADER_BITS      ) << SORT_SHADER_SHIFT)
#define SORT_MATERIAL_MASK     (BIT_MASK(SORT_MATERIAL_BITS    ) << SORT_MATERIAL_SHIFT)
#define SORT_CMD_FLG_MASK      (BIT_MASK(SORT_CMD_FLG_BITS         ) << SORT_CMD_FLG_SHIFT)
#define SORT_TRANSLUCENCY_MASK (BIT_MASK(SORT_TRANSLUCENCY_BITS) << SORT_TRANSLUCENCY_SHIFT)
#define SORT_PASS_MASK         (BIT_MASK(SORT_PASS_BITS        ) << SORT_PASS_SHIFT)
#define SORT_VIEW_MASK         (BIT_MASK(SORT_VIEW_BITS        ) << SORT_VIEW_SHIFT)

#define SORT_COMMAND_BITS  48
#define SORT_COMMAND_SHIFT 0
#define SORT_COMMAND_MASK  (BIT_MASK(SORT_COMMAND_BITS) << SORT_COMMAND_SHIFT)

// Bits add up to 64
TZ_STATIC_ASSERT(64 == SORT_DEPTH_BITS + SORT_MATERIAL_BITS + SORT_CMD_FLG_BITS + SORT_TRANSLUCENCY_BITS + SORT_PASS_BITS + SORT_VIEW_BITS);
TZ_STATIC_ASSERT(64 == SORT_COMMAND_BITS + SORT_CMD_FLG_BITS + SORT_TRANSLUCENCY_BITS + SORT_PASS_BITS + SORT_VIEW_BITS);
// Bit fields don't overlap
TZ_STATIC_ASSERT((0 | SORT_DEPTH_MASK | SORT_MATERIAL_MASK | SORT_CMD_FLG_MASK | SORT_TRANSLUCENCY_MASK | SORT_PASS_MASK | SORT_VIEW_MASK)
                 ==
                 (0 ^ SORT_DEPTH_MASK ^ SORT_MATERIAL_MASK ^ SORT_CMD_FLG_MASK ^ SORT_TRANSLUCENCY_MASK ^ SORT_PASS_MASK ^ SORT_VIEW_MASK));
TZ_STATIC_ASSERT((0 | SORT_COMMAND_MASK | SORT_CMD_FLG_MASK | SORT_TRANSLUCENCY_MASK | SORT_PASS_MASK | SORT_VIEW_MASK)
                 ==
                 (0 ^ SORT_COMMAND_MASK ^ SORT_CMD_FLG_MASK ^ SORT_TRANSLUCENCY_MASK ^ SORT_PASS_MASK ^ SORT_VIEW_MASK));

MaterialID create_material_id(ShaderProgramID program, TextureID* textures, size_t num_textures)
{
  TZ_ASSERT(num_textures < TZ_CONFIG_MAX_NUM_SAMPLERS, "Too many textures!");
  
  const uint32_t texture_hash =  hash::fnv1a((byte*) textures, num_textures           ) & BIT_MASK(SORT_TEXTURE_BITS);
  const uint32_t shader_hash  = (hash::fnv1a((byte*) &program, sizeof(ShaderProgramID)) & BIT_MASK(SORT_SHADER_BITS)) << SORT_TEXTURE_BITS;
  
  return shader_hash | texture_hash;
}

struct DrawCall
{
  ShaderProgramID m_program;
  VertexArrayID m_vao;
  TextureID m_textures[TZ_CONFIG_MAX_NUM_SAMPLERS];
  uint16_t m_num_textures;
    
  uint32_t m_start_index;
  uint32_t m_num_indices;
  uint32_t m_instances;

  DrawType m_draw_type;

  /*UniformHandle uniform_handle*/
};

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

class Frame
{
 public:
  
  Frame(FrameDataBuffer* buffer)
    : m_call_pos(0)
    , m_buffer(buffer)
  {}

  void push_call(Key key, DrawCall* in_dc /*, UniformHandle uniform_handle*/)
  {
    ASSERT(in_dc->m_num_textures, "Too many textures.");
    ASSERT(m_call_pos < TZ_CONFIG_MAX_DRAW_CALLS, "Too many draw calls.");

    DrawCall* new_draw_call = push_struct(DrawCall, m_buffer);
    m_draw_calls[m_call_pos] = new_draw_call;
    *new_draw_call = *in_dc;
    
    RenderSortKey new_key;
    new_key.key = key;
    new_key.index = m_call_pos;
    m_sort_keys[m_call_pos] = new_key;

    m_call_pos++;

    //dc->uniform_handle = uniform_handle
  }

  void draw(/* UniformDatabase& uniform_database */)
  {
    std::sort(m_sort_keys, m_sort_keys + m_call_pos);

    ShaderProgramID m_program = 0;
    VertexArrayID m_vao = 0;
    TextureID m_textures[TZ_CONFIG_MAX_NUM_SAMPLERS];
    uint16_t m_num_textures = 0;

    size_t index;
    Key key;
    KeyData key_data;

    DrawCall* dc;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Probably parameterize this somehow
    
    for (size_t i; i < m_call_pos; i++)
    {
      // Load data
      index = m_sort_keys[i].index;
      key = m_sort_keys[i].key;
      
      key_data.decode_key(key);
      dc = m_draw_calls[index];
      
      { // Check for diff and bind
        if (m_vao != dc->m_vao)
        {
          m_vao = dc->m_vao;
          glBindVertexArray(m_vao);
        }
      
        if (m_program != dc->m_program)
        {
          m_program = dc->m_program;
          glUseProgram(m_program);        
        }
      
        for (int j = 0; j < dc->m_num_textures; j++)
        {
          if (m_textures[j] != dc->m_textures[j])
          {
            m_textures[j] = dc->m_textures[j];
            glActiveTexture(GL_TEXTURE0 + j);
            glBindTexture(GL_TEXTURE_2D, m_textures[j]);
          }
        }
      }

      { // Upload uniforms
        
      }

      { // Draw
        if (dc->m_instances > 0)
        {
          glDrawElementsInstanced(dc->m_draw_type, dc->m_num_indices, GL_UNSIGNED_SHORT, (const void*) (uintptr_t) dc->m_start_index, dc->m_instances);
        }
        else
        {
          glDrawElements(dc->m_draw_type, dc->m_num_indices, GL_UNSIGNED_SHORT, (const void*) (uintptr_t) dc->m_start_index);
        }
      }
    }
  }

  void reset()
  {
    m_call_pos = 0;
  }
    
 private:
  typedef sort::SortKey<Key> RenderSortKey;
  size_t m_call_pos;
  
  DrawCall* m_draw_calls[TZ_CONFIG_MAX_DRAW_CALLS];
  RenderSortKey m_sort_keys[TZ_CONFIG_MAX_DRAW_CALLS];

  FrameDataBuffer* m_buffer;
};

Frame* g_renderer;

void init_rendering()
{
  g_renderer = TZ_push_new(Frame, &g_game_state.main_memory, &g_game_state.frame_memory);
}

void deinit_rendering()
{
  TZ_delete_pushed(Frame, g_renderer);
}

void push_frame()
{
  // Stuff! Probably in the future swap Frame pointers for a render thread
  g_renderer->draw();
  g_renderer->reset();
}

}
}
