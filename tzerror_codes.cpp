#include "tzerror_codes.h"

#include <stdarg.h>
#include <stdlib.h>

namespace tz
{

void check_trace_game_error(GameError err)
{
  if (err)
    error(error_strings[err]);
}

void fatal_game_error(GameError err)
{
  if (err)
    fatal_error(error_strings[err]);
}

void error(const char* fmt, ...)
{
  va_list argptr;

  va_start(argptr, fmt);
  fprintf(stderr, "ERROR: ");
  fprintf(stderr, fmt, argptr);
  fprintf(stderr, "\n");
  va_end(argptr);
}

void fatal_error(const char* fmt, ...)
{
  va_list argptr;
  
  va_start(argptr, fmt);
  fprintf(stderr, "FATAL ERROR: ");
  fprintf(stderr, fmt, argptr);
  fprintf(stderr, "\n");
  va_end(argptr);

  exit(1);
}

}
