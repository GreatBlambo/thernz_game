#pragma once

#include "tzrender_types.h"
#include "tzrendering.h"

namespace tz
{
  namespace renderer
  {
    struct UploadNUniforms;
    struct DrawIndexed;  
    struct DrawIndexedInstanced;

    class IBackend
    {
    public:
      virtual void dispatch(UploadNUniforms* data) = 0;
      virtual void dispatch(DrawIndexed* data) = 0;
      virtual void dispatch(DrawIndexedInstanced* data) = 0;
    private:
      // Backends hold HandleArrays full of their own kinds of data
    };
    
#define TZ_GLOBAL_BACKEND_CALL(type) inline static void function(IBackend* backend, void* data) { backend->dispatch((type*) data); }
  
    ////////////////////////////////////////////////////////////////////////////////
    // Backend commands
    ////////////////////////////////////////////////////////////////////////////////

    struct UploadNUniforms
    {
      TZ_GLOBAL_BACKEND_CALL(UploadNUniforms)
    
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
      TZ_GLOBAL_BACKEND_CALL(DrawIndexed)
    
      VertexArrayID vao;
      DrawType draw_type;
    
      size_t start_index;
      size_t num_indices;
      DataType indices_type;

      Material material;
    };
  
    struct DrawIndexedInstanced
    {
      TZ_GLOBAL_BACKEND_CALL(DrawIndexedInstanced)
    
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
