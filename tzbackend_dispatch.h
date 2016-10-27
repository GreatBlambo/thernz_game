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

    struct UploadNUniforms
    {    
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
      VertexArrayID vao;
      DrawType draw_type;
    
      size_t start_index;
      size_t num_indices;
      DataType indices_type;

      Material material;
    };
  
    struct DrawIndexedInstanced
    {    
      VertexArrayID vao;
      DrawType draw_type;
    
      size_t start_index;
      size_t num_indices;
      size_t instances;
      DataType indices_type;

      Material material;
    };

    ////////////////////////////////////////////////////////////////////////////////
    // Backend functions
    ////////////////////////////////////////////////////////////////////////////////
    void clear_backbuffer(GraphicsBitfield buffer_bits);
  
  }
}
