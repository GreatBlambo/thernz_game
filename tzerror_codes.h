#pragma once

#include <stdio.h>
#include <stdlib.h>

namespace tz
{
  enum GameError
  {
    NO_ERROR,
    ERROR_SDL,
    ERROR_GLEW,
    ERROR_OPENGL,
    ERROR_NULL_PARAM,
    ERROR_BUFFER_SIZE,
    ERROR_INVALID_PARAM,
    ERROR_ALLOC_FAIL,
    ERROR_ARRAY_SIZE
  };
  
  void check_trace_game_error(GameError err);
  void fatal_game_error(GameError err);
  void error(const char* fmt, ...);
  void fatal_error(const char* fmt, ...);

#define TZ_ASSERT(cond, ...) do { if (!cond) { fprintf(stderr, "ASSERT FAILURE: \"" #cond "\"\n"); fprintf(stderr, __VA_ARGS__ ); exit(0);} } while(0)
#define TZ_STATIC_ASSERT(cond, msg) typedef int _static_assert_##msg[(cond) ? 1 : -1];

};
