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
    
#define TZ_BACKEND_DISPATCH_FUNC(name)				\
  static void dispatch(void* data)				\
  {								\
    name* command = (name*) data;				\
    command->backend->dispatch(command);			\
  };

  struct UploadNUniforms
  {
    TZ_BACKEND_DISPATCH_FUNC(UploadNUniforms)
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
  
  struct DrawIndexed
  {
    TZ_BACKEND_DISPATCH_FUNC(DrawIndexed)
    graphics::Backend* backend;
    
    BindingHandle binding;
    DrawType draw_type;
    
    size_t start_index;
    size_t num_indices;
    size_t instances;
    DataType indices_type;

    Material material;
  };

  struct ClearBackbuffer
  {
    TZ_BACKEND_DISPATCH_FUNC(DrawIndexed)
    graphics::Backend* backend;

    GraphicsBitfield bitfield;
  };  
}
}
