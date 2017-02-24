#pragma once


#define TZ_ARRAY_SIZE(x) sizeof(x)/sizeof(*x)

#define TZ_BIT_MASK(bits) ((1ull << bits) - 1)
#define TZ_MASK_GET(val, width, lsb) (val >> lsb) & TZ_BIT_MASK(width);

#define TZ_MACRO_CAT_(a, b) a ## b
#define TZ_MACRO_CAT(a, b) TZ_MACRO_CAT_(a, b)
