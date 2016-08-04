#include "memory.h"
#include "stdint.h"

static inline void* align_forward(void* ptr, size_t align = DEFAULT_ALIGN)
{
  return (void*) ((uintptr_t(ptr) + align - 1) & ~(align - 1));
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
