#pragma once

#include <stdint.h>

#include <memory.h>

#define HANDLE_INDEX_BITS 22
#define HANDLE_INDEX_MASK (1 << HANDLE_INDEX_BITS) - 1

#define HANDLE_GENERATION_BITS 10
#define HANDLE_GENERATION_MASK (1 << HANDLE_GENERATION_BITS) - 1

typedef uint32_t Handle;

inline uint32_t handle_get_index(Handle handle)
{
  return handle & HANDLE_INDEX_MASK;
}

inline uint16_t handle_get_generation(Handle handle)
{
  return (handle >> HANDLE_INDEX_BITS) & HANDLE_GENERATION_MASK;
}

inline Handle generate_handle(uint32_t index, uint16_t generation)
{
  return index | (((uint32_t) generation) << HANDLE_INDEX_BITS);
}

struct HandleArray
{
  uint16_t* generations;
  size_t* free_indices;
  size_t num_handles;
  size_t max_handles;

  size_t next_free_index;
  size_t num_gaps;

  size_t min_free_indicies;
};

void create_handle_array(HandleArray* handle_array, const size_t max_handles, Buffer* buffer);
bool new_handle(HandleArray* handle_array, Handle* handle);
void deactivate_handle(HandleArray* handle_array, Handle handle);
bool handle_is_active(HandleArray* handle_array, Handle handle);
