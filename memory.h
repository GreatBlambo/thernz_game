#pragma once
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <atomic>

#include "error_codes.h"
#define DEFAULT_ALIGN 16

#define KILOBYTE(x) 1024u * x
#define MEGABYTE(x) 1024u * KILOBYTE(x)
#define GIGABYTE(x) 1024u * MEGABYTE(x)

#define ARRAY_SIZE(x) sizeof(x)/sizeof(*x)

////////////////////////////////////////////////////////////////////////////////
// Types
////////////////////////////////////////////////////////////////////////////////

typedef uint8_t byte;

////////////////////////////////////////////////////////////////////////////////
// Mem utils
////////////////////////////////////////////////////////////////////////////////

inline void* align_forward(void* ptr, size_t align = DEFAULT_ALIGN)
{
  return (void*) ((uintptr_t(ptr) + align - 1) & ~(align - 1));
}

////////////////////////////////////////////////////////////////////////////////
// Buffers
////////////////////////////////////////////////////////////////////////////////

template <typename OffsetType>
struct GenericBuffer
{
  void* start;
  size_t size;
  OffsetType offset;
};

template <typename OffsetType>
GameError create_buffer(GenericBuffer<OffsetType>* buffer, void* data, size_t size)
{
  if (!buffer || !data)
    return ERROR_NULL_PARAM;
  buffer->start = data;
  buffer->size = size;
  buffer->offset = 0;
  return NO_ERROR;
}

template <typename OffsetType>
void* push_size(GenericBuffer<OffsetType>* buffer, size_t size, size_t align = DEFAULT_ALIGN)
{
  if (!buffer)
  {
    return NULL;
  }

  void* result = align_forward(buffer_get_offset_pointer(buffer), align);
  if (!size)
    return result;

  size_t new_offset = ((byte*) result - (byte*) buffer->start) + size;
  if (new_offset > buffer->size)
  {
    return NULL;
  }

  buffer->offset = new_offset;
  return result;
}

template <typename OffsetType, typename OtherOffsetType>
inline GameError push_buffer(GenericBuffer<OffsetType>* source, GenericBuffer<OtherOffsetType>* result, size_t size, size_t align = DEFAULT_ALIGN)
{
  return create_buffer(result, push_size(source, size, align), size);
}

#define zero_buffer(buffer) memset(buffer->start, 0, buffer->size)
#define push_array(T, buffer, length) (T*) push_size(buffer, sizeof(T) * length)
#define zero_array(array, length) memset(array, 0, sizeof(*array) * length)
#define push_struct(T, buffer) push_array(T, buffer, 1)

template <typename OffsetType>
inline size_t buffer_get_remaining(const GenericBuffer<OffsetType>* buffer)
{
  return buffer->size - buffer->offset;
}

template <typename OffsetType>
inline void* buffer_get_offset_pointer(const GenericBuffer<OffsetType>* buffer)
{
  return (void*) ((byte*) buffer->start + buffer->offset);
}

template <typename OffsetType>
inline void buffer_reset(GenericBuffer<OffsetType>* buffer) 
{
  buffer->offset = 0;
}

typedef GenericBuffer<size_t> Buffer;
typedef GenericBuffer<std::atomic_size_t> FrameDataBuffer;
