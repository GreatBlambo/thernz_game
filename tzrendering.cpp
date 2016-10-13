#include <stdint.h>
#include "tzengine.h"

namespace tz
{
/**
 * Sorts and submits draw calls to the GPU. That's all this module does for now. We can sort out
 * solutions for thread safety and double buffering later.
 * 
 */
namespace renderer
{
  /**
   * Draw call parameters: render target, pass (translucency, depth, etc), material, buffer, and object
   * The sorting depends on this magnitude:
   * render target > pass > shader program > texture > vao > object
   * 
   * An object is a range on a vao and an instance count.
   */

  typedef uint64_t Key;
  
  void init(void* buffer, size_t size)
  {
    // initialize rendering globals. Pass in buffer for queue
  }

  void deinit()
  {
    // deinit rendering globals
  }
  
  void push_frame()
  {
    
  }
};
  
};
