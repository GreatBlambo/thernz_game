#pragma once
#include "tzmemory.h"
#include "tzshared.h"

#define TZ_BITFIELD_START(name, type)		\
  template <typename T = type>			\
  struct _##name				\
  {						\
  _##name() : val(0) {}				\
  T val;

#define TZ_BITFIELD_ENTRY(name, bits, shift)				\
  TZ_STATIC_ASSERT(bits < sizeof(T) * 8, bit##name);			\
  TZ_STATIC_ASSERT(shift < sizeof(T) * 8, shift##name);			\
  inline T name() { return TZ_MASK_GET(val, bits, shift); }		\
  inline void set_##name(T value) { val |= (value << shift); }		\
  static constexpr max_##name = 1 << bits;				\
  static constexpr bits_##name = bits;					\

#define TZ_BITFIELD_END(name)			\
  };						\
  typedef _##name<> name;
