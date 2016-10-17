#pragma once
#include <stdint.h>

namespace tz
{

namespace sort
{

template <typename T>
  struct SortKey
  {
    bool operator < (const SortKey<T>& other) const
    {
      return key < other.key;
    }
    T key;
    size_t index;
  };
}

}
