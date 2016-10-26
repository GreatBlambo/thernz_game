#pragma once

#include "tzmemory.h"

namespace tz
{
  class SlabAllocator : public foundation::Allocator
  {
  public:
    SlabAllocator(size_t initial_size, foundation::Allocator& backing_allocator);
    
    void* allocate(uint32_t size, uint32_t align = DEFAULT_ALIGN);
    void deallocate(void* p);
    uint32_t allocated_size(void* p);
    uint32_t total_allocated();
    
  private:
    foundation::Allocator& m_backing_alloc;
    Buffer m_buffer;
  };
};
