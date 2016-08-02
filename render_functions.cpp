#include <glm/glm.hpp>

#include "render_functions.h"

void clear_color(Color* color)
{
  if (!color)
    return;
  glClearColor(color->r, color->g, color->b, color->a);
  glClear(GL_COLOR_BUFFER_BIT);
}
