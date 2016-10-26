#include "tzmemory.h"

namespace tz
{
LinearAllocator::LinearAllocator(foundation::Allocator& alloc, size_t size)
  : m_alloc(alloc)
{
  fatal_game_error(create_buffer(&m_buffer, alloc.allocate(size), size));
}

LinearAllocator::~LinearAllocator()
{
  m_alloc.deallocate(m_buffer.start);
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

void LinearAllocator::reset()
{
  buffer_reset(&m_buffer);
}

////////////////////////////////////////////////////////////////////////////////
  
AtomicLinearAllocator::AtomicLinearAllocator(foundation::Allocator& alloc, size_t size)
  : m_alloc(alloc)
{
  m_data = (byte*) alloc.allocate(size);
  TZ_ASSERT(m_data, "Alloc failed\n");
  m_size = size;
  m_offset = 0;
}

AtomicLinearAllocator::~AtomicLinearAllocator()
{
  m_alloc.deallocate((void*) m_data);
}

// TODO: Alignment?
void* AtomicLinearAllocator::allocate(uint32_t size, uint32_t align)
{
  if (size > m_size)
    return NULL;
  m_offset += size;
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

void AtomicLinearAllocator::reset()
{
  m_offset = 0;
}
}
