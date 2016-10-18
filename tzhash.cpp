#include "tzmemory.h"

#define FNV_OFFSET 14695981039346656037u
#define FNV_PRIME 1099511628211u

namespace tz
{

namespace hash
{
  uint64_t fnv1a (const byte* buffer, const size_t n_bytes)
  {
    uint64_t hash = FNV_OFFSET;
    for (int i = 0; i < n_bytes; i++)
    {
      hash = hash ^ buffer[i];
      hash = hash * FNV_PRIME;
    }
    return hash;
  }
};
  
};
