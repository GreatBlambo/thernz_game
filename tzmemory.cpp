#include "tzmemory.h"

namespace tz
{
  LinearAllocator::LinearAllocator(void* data, size_t size)
  {
    create_buffer(&m_buffer, data, size);
  }

  void* LinearAllocator::allocate(uint32_t size, uint32_t align)
  {
    return push_size(&m_buffer, size, align);
  }

  void LinearAllocator::deallocate(void* p)
  {
    return;
  }

  uint32_t LinearAllocator::allocated_size(void* p)
  {
    return foundation::Allocator::SIZE_NOT_TRACKED;
  }

  uint32_t LinearAllocator::total_allocated()
  {
    return m_buffer.offset;
  }

  ////////////////////////////////////////////////////////////////////////////////
  
  AtomicLinearAllocator::AtomicLinearAllocator(void* data, size_t size)
  {
    m_data = (byte*) data;
    m_size = size;
  }

  void* AtomicLinearAllocator::allocate(uint32_t size, uint32_t align)
  {
    if (m_offset + size > m_size) return NULL;
    m_offset.fetch_add(size, std::memory_order_release);
    return (void*) (m_data + m_offset);
  }

  void AtomicLinearAllocator::deallocate(void* p)
  {
    return;
  }

  uint32_t AtomicLinearAllocator::allocated_size(void* p)
  {
    return foundation::Allocator::SIZE_NOT_TRACKED;
  }

  uint32_t AtomicLinearAllocator::total_allocated()
  {
    return (uint32_t) m_offset;
  }
}
