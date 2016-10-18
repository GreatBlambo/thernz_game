#include "tzbackend_dispatch.h"
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

namespace tz
{
namespace renderer
{ 
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

TZ_BACKEND_DISPATCH_IMPL(BindMaterial)
{
  static BindMaterial s_material = {0};
  BindMaterial* material_data = (BindMaterial*) data;
  
  if (s_material.shader != material_data->shader)
  {
    s_material.shader = material_data->shader;
    glUseProgram(s_material.shader);
  }
  
  for (size_t i = 0; i < material_data->num_textures; i++)
  {
    if (s_material.textures[i].texture_id != material_data->textures[i].texture_id)
    {
      s_material.textures[i] = material_data->textures[i];
      glActiveTexture(GL_TEXTURE0 + i);
      glBindTexture(s_material.textures[i].texture_id, s_material.textures[i].type);
    }
  }
}
  
TZ_BACKEND_DISPATCH_IMPL(DrawIndexed)
{
  DrawIndexed* command = (DrawIndexed*) data;
  glDrawElements(command->draw_type, command->num_indices, command->indices_type, (const void*) command->start_index);
}
  
TZ_BACKEND_DISPATCH_IMPL(DrawIndexedInstanced)
{
  DrawIndexedInstanced* command = (DrawIndexedInstanced*) data;
  glDrawElementsInstanced(command->draw_type, command->num_indices, command->indices_type, (const void*) command->start_index, command->instances);
}
  
}
}
