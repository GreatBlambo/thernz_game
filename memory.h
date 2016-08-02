#pragma once
#include <cstddef>

struct Buffer
{
  void* start;
  size_t size;
  size_t offset;
};

size_t buffer_get_remaining(const Buffer* buffer);
void* buffer_get_offset_pointer(const Buffer* buffer);
