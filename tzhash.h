#pragma once

#include "memory.h"

namespace tz
{
namespace hash
{

uint64_t fnv1a (const byte* buffer, const size_t n_bytes);

};
};
