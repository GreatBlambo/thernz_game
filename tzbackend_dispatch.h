#pragma once

#include "tzrender_types.h"
#include "tzcommand_buffer.h"

namespace tz
{
namespace renderer
{
  ////////////////////////////////////////////////////////////////////////////////
  // Backend commands
  ////////////////////////////////////////////////////////////////////////////////
    
#define TZ_BACKEND_DISPATCH_FUNC static Command::DispatchFunction dispatch;

  struct UploadNUniforms
  {
    TZ_BACKEND_DISPATCH_FUNC
    
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
    TZ_BACKEND_DISPATCH_FUNC
    
    VertexArrayID vao;
    DrawType draw_type;
    
    size_t start_index;
    size_t num_indices;
    size_t instances;
    DataType indices_type;

    Material material;
  };

  
#define TZ_BACKEND_DISPATCH_API_DECL(command_name, backend)		\
  static void _##command_name(void* data);				
  
#define TZ_GRAPHICS_BACKEND(backend)					\
  TZ_BACKEND_DISPATCH_API_DECL(UploadNUniforms, backend);		\
  TZ_BACKEND_DISPATCH_API_DECL(DrawIndexed, backend);			\
  static void bind_commands()						\
  {									\
    UploadNUniforms::dispatch = backend::_UploadNUniforms;		\
    DrawIndexed::dispatch = backend::_DrawIndexed;			\
  }

  
  
}
}
