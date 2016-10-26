#include "tzallocator_types.h"

namespace tz
{
  SlabAllocator::SlabAllocator(size_t minimum_size, foundation::Allocator& backing_allocator)
    : m_backing_alloc(backing_allocator)
  {}
};
