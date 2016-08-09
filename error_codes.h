#pragma once

#include <stdio.h>

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

static const char* error_strings[] =
{
  "No error"
  , "SDL"
  , "GLEW"
  , "OPENGL"
  , "Null parameter"
  , "Buffer size"
  , "Invalid parameter"
  , "Allocation failure"
  , "Array size"
};

void check_trace_game_error(GameError err);
void fatal_game_error(GameError err);
void error(const char* fmt, ...);
void fatal_error(const char* fmt, ...);
