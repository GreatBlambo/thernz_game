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

  extern void (*clear_backbuffer)(GraphicsBitfield);
  
#define TZ_BACKEND_DISPATCH_API_DECL(command_name, backend)		\
  static void _##command_name(void* data);

#define TZ_BACKEND_FREE_FUNCTION_DECL(return_type, name, ...)	\
  static return_type _##name(__VA_ARGS__);

#define TZ_BACKEND_DISPATCH_API_IMPL(api, command_name)			\
  void api::_##command_name(void* data)					
#define TZ_BACKEND_FREE_FUNCTION_IMPL(api, return_type, name, ...)	\
  return_type api::_##name(__VA_ARGS__)
  
#define TZ_GRAPHICS_BACKEND(backend)					\
  struct backend							\
  {									\
    TZ_BACKEND_DISPATCH_API_DECL(UploadNUniforms, backend);		\
    TZ_BACKEND_DISPATCH_API_DECL(DrawIndexed, backend);			\
    TZ_BACKEND_FREE_FUNCTION_DECL(void, clear_backbuffer, GraphicsBitfield bitfield); \
    static void bind_commands()						\
    {									\
      UploadNUniforms::dispatch = backend::_UploadNUniforms;		\
      DrawIndexed::dispatch = backend::_DrawIndexed;			\
      clear_backbuffer = backend::_clear_backbuffer;			\
    }									\
  };
  
}
}
