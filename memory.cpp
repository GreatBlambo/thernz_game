#include "memory.h"
#include "stdint.h"

size_t buffer_get_remaining(const Buffer* buffer)
{
  return buffer->size - buffer->offset;
}

void* buffer_get_offset_pointer(const Buffer* buffer)
{
  return (void*) ((uint8_t*) buffer->start + buffer->offset);
}

inline void* align_forward(void* ptr, size_t align = DEFAULT_ALIGN)
{
  uintptr_t p = uintptr_t(ptr);
  const uint32_t mod = p % align;
  if (mod)
  {
    p -=  mod;
  }
  return (void*) p;
}

GameError create_buffer(Buffer* buffer, void* data, size_t size)
{
  if (!buffer || !data)
    return ERROR_NULL_PARAM;
  buffer->start = data;
  buffer->size = size;
  buffer->offset = 0;
  return NO_ERROR;
}

void* push_size(Buffer* buffer, size_t size, size_t align)
{
  if (!buffer)
  {
    return NULL;
  }

  void* result = align_forward(buffer_get_offset_pointer(buffer), align);
  if (!size)
    return result;

  size_t new_offset = ((uint8_t*) result - (uint8_t*) buffer->start) + size;
  if (new_offset > buffer->size)
  {
    return NULL;
  }

  buffer->offset = new_offset;
  return result;
}

inline void buffer_reset(Buffer* buffer)
{
  buffer->offset = 0;
}
