#pragma once
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <atomic>

#include <thirdparty/bitsquid-foundation-git/memory.h>

#include "tzerror_codes.h"

namespace tz
{
  
#define TZ_DEFAULT_ALIGN 16

#define TZ_KILOBYTE(x) 1024u * x
#define TZ_MEGABYTE(x) 1024u * TZ_KILOBYTE(x)
#define TZ_GIGABYTE(x) 1024u * TZ_MEGABYTE(x)

#define TZ_ARRAY_SIZE(x) sizeof(x)/sizeof(*x)

#define TZ_BIT_MASK(bits) ((1ull << bits) - 1)
#define TZ_MASK_GET(val, width, lsb) (val >> lsb) & TZ_BIT_MASK(width);

////////////////////////////////////////////////////////////////////////////////
// Types
////////////////////////////////////////////////////////////////////////////////

typedef uint8_t byte;

////////////////////////////////////////////////////////////////////////////////
// Mem utils
////////////////////////////////////////////////////////////////////////////////

inline void* align_forward(void* ptr, size_t align = TZ_DEFAULT_ALIGN)
{
  return (void*) ((uintptr_t(ptr) + align - 1) & ~(align - 1));
}

////////////////////////////////////////////////////////////////////////////////
// Buffers
////////////////////////////////////////////////////////////////////////////////

struct Buffer
{
  void* start;
  size_t size;
  size_t offset;
};

inline GameError create_buffer(Buffer* buffer, void* data, size_t size)
{
  if (!buffer || !data)
    return ERROR_NULL_PARAM;
  buffer->start = data;
  buffer->size = size;
  buffer->offset = 0;
  return NO_ERROR;
}

inline size_t buffer_get_remaining(const Buffer* buffer)
{
  return buffer->size - buffer->offset;
}


inline void* buffer_get_offset_pointer(const Buffer* buffer)
{
  return (void*) ((byte*) buffer->start + buffer->offset);
}


inline void buffer_reset(Buffer* buffer) 
{
  buffer->offset = 0;
}

inline void* push_size(Buffer* buffer, size_t size, size_t align = TZ_DEFAULT_ALIGN)
{
  if (!buffer)
  {
    return NULL;
  }

  void* result = align_forward(buffer_get_offset_pointer(buffer), align);
  if (!size)
    return result;

  size_t new_offset = ((byte*) result - (byte*) buffer->start) + size;
  if (buffer->size < new_offset)
  {
    return NULL;
  }

  buffer->offset = new_offset;
  
  return result;
}

inline GameError push_buffer(Buffer* source, Buffer* result, size_t size, size_t align = TZ_DEFAULT_ALIGN)
{
  return create_buffer(result, push_size(source, size, align), size);
}

#define TZ_zero_buffer(buffer) memset(buffer->start, 0, buffer->size)
#define TZ_push_array(T, buffer, length) (T*) push_size(buffer, sizeof(T) * length, alignof(T))
#define TZ_zero_array(array, length) memset(array, 0, sizeof(*array) * length)
#define TZ_push_struct(T, buffer) TZ_push_array(T, buffer, 1)
#define TZ_push_new(T, buffer, ...) (new (push_size(buffer, sizeof(T), alignof(T))) T(__VA_ARGS__))
#define TZ_delete_pushed(T, ptr) ptr->~T()

////////////////////////////////////////////////////////////////////////////////
// Linear Allocator
// Does a simple pointer bump on a buffer
////////////////////////////////////////////////////////////////////////////////

class LinearAllocator : public foundation::Allocator
{
public:
  LinearAllocator(foundation::Allocator& alloc, size_t size);
  ~LinearAllocator();

  void* allocate(uint32_t size, uint32_t align = foundation::Allocator::DEFAULT_ALIGN);

  void deallocate(void* p);

  uint32_t allocated_size(void* p);

  uint32_t total_allocated();

  void reset();
  
private:
  Buffer m_buffer;
  foundation::Allocator& m_alloc;
};

class AtomicLinearAllocator : public foundation::Allocator
{
public:
  AtomicLinearAllocator(foundation::Allocator& alloc, size_t size);
  ~AtomicLinearAllocator();

  void* allocate(uint32_t size, uint32_t align = foundation::Allocator::DEFAULT_ALIGN);

  void deallocate(void* p);

  uint32_t allocated_size(void* p);

  uint32_t total_allocated();

  void reset();
    
private:
  std::atomic_size_t m_offset;
  size_t m_size;
  byte* m_data;

  foundation::Allocator& m_alloc;
};

}
