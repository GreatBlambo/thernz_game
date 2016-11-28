#pragma once

#include "tzrender_types.h"
#include "tzcommand_buffer.h"
#include "tzbackend_types.h"

namespace tz
{
namespace renderer
{
  ////////////////////////////////////////////////////////////////////////////////
  // Backend commands
  ////////////////////////////////////////////////////////////////////////////////
  
#define TZ_BACKEND_DISPATCH_FUNC(name, def)				\
    virtual void dispatch(name* command) = 0;                           \
    struct name
    
  class Backend
  {
  public:      
    virtual void init() = 0;
    virtual void deinit() = 0;
    
    virtual void set_window_name(const char* name) = 0;
    virtual void set_window_size(int width, int height) = 0;
    virtual void set_window_fullscreen() = 0;
    virtual void set_window_fullscreen_windowed() = 0;
    virtual void set_window_windowed() = 0;
    virtual void set_window_position(int x, int y) = 0;
    
    virtual void swap_backbuffer() = 0;
    
    virtual ShaderHandle load_shader(const char* pathname, ShaderType shader_type) = 0;
    virtual void destroy_shader(ShaderHandle shader) = 0;
    
    virtual ProgramHandle link_program(ShaderHandle* shaders, size_t num_shaders,
                                       const VertexAttribArray& vert_spec) = 0;
    virtual void destroy_program(ProgramHandle program) = 0;
    
    virtual Texture load_texture(const char* pathname, int* width, int* height) = 0;
    virtual void destroy_texture(Texture& texture) = 0;
    
    virtual BufferHandle create_buffer(void* data, size_t data_size, BufferUsage usage) = 0;
    virtual void update_buffer(BufferHandle handle, size_t offset, void* data, size_t data_size) = 0;
    virtual void destroy_buffer(BufferHandle handle) = 0;
    
    virtual BindingHandle create_binding(const VertexAttribArray& vert_spec) = 0;
    virtual void destroy_binding(BindingHandle handle) = 0;

    template <typename T>
    static void dispatch(void* data)                                  
    {                                                                 
      T* command = (T*) data;
      command->backend->dispatch(command);
    }

    TZ_BACKEND_DISPATCH_FUNC(SetPipelineState)
    {
      graphics::Backend* backend;
      // Shaders
      ProgramHandle shader_stages;  
      // Vertex Format
      BindingHandle vertex_format;

      // Per-sample stuff
      bool depth_test;
    };

    TZ_BACKEND_DISPATCH_FUNC(SetBuffer)
    {
      graphics::Backend* backend;
      BufferType buffer_type;
      BufferHandle buffer_handle;
    };
      
    TZ_BACKEND_DISPATCH_FUNC(UploadNUniforms)
    {
      graphics::Backend* backend;
      bool transpose;
      int location;
      size_t n;
      enum Type
      {
          INT,
          FLOAT,
          VEC2,
          VEC3,
          VEC4,
          MAT4
      } type;    
    };
    
    TZ_BACKEND_DISPATCH_FUNC(struct DrawIndexed)
    {
      graphics::Backend* backend;
      BindingHandle binding;
      DrawType draw_type;
      
      size_t start_index;
      size_t num_indices;
      size_t instances;
      DataType indices_type;
    };
    
    TZ_BACKEND_DISPATCH_FUNC(ClearBackbuffer)
    {
      graphics::Backend* backend;
      GraphicsBitfield bitfield;
    };  
  }
}
