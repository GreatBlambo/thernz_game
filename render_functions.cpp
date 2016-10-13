#include <glm/glm.hpp>

#include "render_functions.h"

void clear_color(Color* color)
{
  if (!color)
    return;
  glClearColor(color->x, color->y, color->z, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

////////////////////////////////////////////////////////////////////////////////
// UBOs
////////////////////////////////////////////////////////////////////////////////

void shader_bind_ubo_block(ShaderProgramID program, const char* name, GLuint location)
{
  GLuint uniform_block_index = glGetUniformBlockIndex(program, name);  
  glUniformBlockBinding(program, uniform_block_index, location);
}

UniformBufferID create_ubo(size_t size, GLuint location, GLenum update_rate)
{
  UniformBufferID ubo;
  glGenBuffers(1, &ubo);

  glBindBuffer(GL_UNIFORM_BUFFER, ubo);
  glBufferData(GL_UNIFORM_BUFFER, size, NULL, update_rate);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);

  glBindBufferRange(GL_UNIFORM_BUFFER, location, ubo, 0, size);

  return ubo;
}

void delete_ubo(UniformBufferID ubo)
{
  glDeleteBuffers(1, &ubo);
}

UniformBufferID create_ubo_dynamic(size_t size, GLuint location)
{
  create_ubo(size, location, GL_DYNAMIC_DRAW);
}

UniformBufferID create_ubo_static(size_t size, GLuint location)
{
  create_ubo(size, location, GL_STATIC_DRAW);
}

void upload_ubo_data(UniformBufferID ubo, size_t offset, size_t size, void* data)
{
  glBindBuffer(GL_UNIFORM_BUFFER, ubo);
  glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
