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
