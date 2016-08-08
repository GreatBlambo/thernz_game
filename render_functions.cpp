#include <glm/glm.hpp>

#include "render_functions.h"

void clear_color(Color* color)
{
  if (!color)
    return;
  glClearColor(color->x, color->y, color->z, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
