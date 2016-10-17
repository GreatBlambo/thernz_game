#pragma once
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <atomic>

#include "tzerror_codes.h"

namespace tz
{
  
#define TZ_DEFAULT_ALIGN 16

#define TZ_KILOBYTE(x) 1024u * x
#define TZ_MEGABYTE(x) 1024u * TZ_KILOBYTE(x)
#define TZ_GIGABYTE(x) 1024u * TZ_MEGABYTE(x)

#define TZ_ARRAY_SIZE(x) sizeof(x)/sizeof(*x)

#define TZ_BIT_MASK(bits) ((1ull << bits) - 1)

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
void* push_size(GenericBuffer<OffsetType>* buffer, size_t size, size_t align = TZ_DEFAULT_ALIGN)
{
  if (!buffer)
  {
    return NULL;
  }

  void* result = align_forward(buffer_get_offset_pointer(buffer), align);
  if (!size)
    return result;

  size_t new_offset = ((byte*) result - (byte*) buffer->start) + size;
  printf("new_offset = %u, buffer size = %u\n", new_offset, buffer->size);
  printf("%d\n", buffer->size < new_offset);
  if (buffer->size < new_offset)
  {
    return NULL;
  }

  buffer->offset = new_offset;
  
  return result;
}

template <typename OffsetType, typename OtherOffsetType>
inline GameError push_buffer(GenericBuffer<OffsetType>* source, GenericBuffer<OtherOffsetType>* result, size_t size, size_t align = TZ_DEFAULT_ALIGN)
{
  return create_buffer(result, push_size(source, size, align), size);
}

#define TZ_zero_buffer(buffer) memset(buffer->start, 0, buffer->size)
#define TZ_push_array(T, buffer, length) (T*) push_size(buffer, sizeof(T) * length, alignof(T))
#define TZ_zero_array(array, length) memset(array, 0, sizeof(*array) * length)
#define TZ_push_struct(T, buffer) push_array(T, buffer, 1)
#define TZ_push_new(T, buffer, ...) new (push_size(buffer, sizeof(T), alignof(T))) T(__VA_ARGS__)
#define TZ_delete_pushed(T, ptr) ptr->~T()

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

////////////////////////////////////////////////////////////////////////////////
// Arrays
////////////////////////////////////////////////////////////////////////////////

template <typename T>
struct Array
{
  T* start;
  size_t size;
  size_t capacity;
};

template <typename T>
void push_array_struct(Array<T>* array, Buffer* buffer, size_t capacity)
{
  array->start = TZ_push_array(T, buffer, capacity);
  array->capacity = capacity;
  array->size = 0;
  return array;
}

}
