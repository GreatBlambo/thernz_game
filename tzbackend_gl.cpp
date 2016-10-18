#include "tzbackend_dispatch.h"
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

namespace tz
{
namespace renderer
{

static void bind_material(Material& material)
{
  static Material s_material = {0};
  
  if (s_material.shader != material.shader)
  {
    s_material.shader = material.shader;
    glUseProgram(s_material.shader);
  }
  
  for (size_t i = 0; i < material.num_textures; i++)
  {
    if (s_material.textures[i].texture_id != material.textures[i].texture_id)
    {
      s_material.textures[i] = material.textures[i];
      glActiveTexture(GL_TEXTURE0 + i);
      glBindTexture(s_material.textures[i].texture_id, s_material.textures[i].type);
    }
  }
};

static void bind_vao(VertexArrayID vao)
{
  static VertexArrayID s_vao = 0;
  if (s_vao != vao)
  {
    s_vao = vao;
    glBindVertexArray(s_vao);
  }
}

static size_t get_ogl_type_size(DataType type)
{
  switch(type)
  {
   case (UNSIGNED_BYTE):
     return sizeof(GLubyte);
     break;
   case (UNSIGNED_SHORT):
     return sizeof(GLushort);
     break;
   case (UNSIGNED_INT):
     return sizeof(GLuint);
     break;
  };
}

////////////////////////////////////////////////////////////////////////////////
// Backend commands
////////////////////////////////////////////////////////////////////////////////

TZ_BACKEND_DISPATCH_IMPL(UploadNUniforms)
{
  UploadNUniforms* uniform_command = (UploadNUniforms*) data;
  void* uniform_data = Command::get_aux_data(data);
    
  switch(uniform_command->type)
  {
   case UploadNUniforms::INT:
     glUniform1iv(uniform_command->location, uniform_command->n, (const GLint*) uniform_data);
     break;
   case UploadNUniforms::FLOAT:
     glUniform1fv(uniform_command->location, uniform_command->n, (const GLfloat*) uniform_data);
   case UploadNUniforms::VEC2:
     glUniform2fv(uniform_command->location, uniform_command->n, (const GLfloat*) uniform_data);
     break;
   case UploadNUniforms::VEC3:
     glUniform3fv(uniform_command->location, uniform_command->n, (const GLfloat*) uniform_data);
     break;
   case UploadNUniforms::MAT4:
     glUniformMatrix4fv(uniform_command->location, uniform_command->n, transpose, (const GLfloat*) uniform_data);
     break;
  }
}

TZ_BACKEND_DISPATCH_IMPL(DrawIndexed)
{
  DrawIndexed* command = (DrawIndexed*) data;

  bind_vao(command->vao);
  bind_material(command->material);
  
  glDrawElements(command->draw_type, command->num_indices, command->indices_type, (const void*) (get_ogl_type_size(command->indices_type) * command->start_index));
}
  
TZ_BACKEND_DISPATCH_IMPL(DrawIndexedInstanced)
{
  DrawIndexedInstanced* command = (DrawIndexedInstanced*) data;

  bind_vao(command->vao);
  bind_material(command->material);
  
  glDrawElementsInstanced(command->draw_type, command->num_indices, command->indices_type, (const void*) (get_ogl_type_size(command->indices_type) * command->start_index), command->instances);
}
  
}
}
