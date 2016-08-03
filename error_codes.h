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

#define CHECK_ERR_TRACE_RETURN(err, call) if (err = call) { printf("Error! Code: %i, when calling " #call "\n", err); return err; }
#define CHECK_ERR_RETURN(err, call) if (err = call) { return err; }
