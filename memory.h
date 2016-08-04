#pragma once
#include <stddef.h>
#include <stdint.h>

#include "error_codes.h"
#define DEFAULT_ALIGN 4

#define KILOBYTE(x) 1024u * x
#define MEGABYTE(x) 1024u * KILOBYTE(x)
#define GIGABYTE(x) 1024u * MEGABYTE(x)

struct Buffer
{
  void* start;
  size_t size;
  size_t offset;
};

GameError create_buffer(Buffer* buffer, void* data, size_t size);
void* push_size(Buffer* buffer, size_t size, size_t align = DEFAULT_ALIGN);
inline GameError push_buffer(Buffer* source, Buffer* result, size_t size, size_t align = DEFAULT_ALIGN)
{
  return create_buffer(result, push_size(source, size, align), size);
}

template <typename T>
inline T* push_struct(Buffer* source, size_t align = DEFAULT_ALIGN)
{
  return (T*) push_size(source, sizeof(T), align);
}

template <typename T>
inline T* push_array(Buffer* source, size_t length, size_t align = DEFAULT_ALIGN)
{
  return (T*) push_size(source, sizeof(T) * length, align);
}

inline size_t buffer_get_remaining(const Buffer* buffer)
{
  return buffer->size - buffer->offset;
}

inline void* buffer_get_offset_pointer(const Buffer* buffer)
{
  return (void*) ((uint8_t*) buffer->start + buffer->offset);
}

inline void buffer_reset(Buffer* buffer) 
{
  buffer->offset = 0;
}
