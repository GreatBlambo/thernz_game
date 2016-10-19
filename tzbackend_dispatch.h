#pragma once

#include "tzrender_types.h"
#include "tzrendering.h"

namespace tz
{
namespace renderer
{
  ////////////////////////////////////////////////////////////////////////////////
  // Backend commands
  ////////////////////////////////////////////////////////////////////////////////

#define TZ_BACKEND_DISPATCH_IMPL(command_name) void command_name::function(void* data)
#define TZ_BACKEND_DISPATCH_DECL void function(void* data);

  struct UploadNUniforms
  {
    TZ_BACKEND_DISPATCH_DECL
    
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
    TZ_BACKEND_DISPATCH_DECL
    
    VertexArrayID vao;
    DrawType draw_type;
    
    size_t start_index;
    size_t num_indices;
    DataType indices_type;

    Material material;
  };
  
  struct DrawIndexedInstanced
  {
    TZ_BACKEND_DISPATCH_DECL
    
    VertexArrayID vao;
    DrawType draw_type;
    
    size_t start_index;
    size_t num_indices;
    size_t instances;
    DataType indices_type;

    Material material;
  };
}
}
