#pragma once

#include "tzgraphics.h"

#ifndef TZ_CONFIG_COMMAND_BUFFER_MAX_SIZE
#define TZ_CONFIG_COMMAND_BUFFER_MAX_SIZE (64 << 10)
#endif

#ifndef TZ_CONFIG_MAX_UNIFORMS
#define TZ_CONFIG_MAX_UNIFORMS ((64 << 10) - 1)
#endif

#ifndef TZ_CONFIG_MAX_DRAW_CALLS
#define TZ_CONFIG_MAX_DRAW_CALLS ((64 << 10) - 1)
#endif

#ifndef TZ_CONFIG_MAX_NUM_SAMPLERS
#define TZ_CONFIG_MAX_NUM_SAMPLERS 16
#endif
