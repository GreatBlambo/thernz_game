#pragma once

#include "tzbackend_dispatch.h"

namespace tz
{
  namespace renderer
  {
    class GLBackend : public IBackend
    {
    public:
      void dispatch(UploadNUniforms* data);  
      void dispatch(DrawIndexed* data);  
      void dispatch(DrawIndexedInstanced* data);  
    private:
      // HandleArrays for stuff
    };

  }
}
