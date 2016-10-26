#include "tzbackend_dispatch.h"
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "tzbackend_types.h"

namespace tz
{
namespace renderer
{

void clear_backbuffer(GraphicsBitfield buffer_bits)
{
  glClear(buffer_bits);
}

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

////////////////////////////////////////////////////////////////////////////////
// Backend
////////////////////////////////////////////////////////////////////////////////

void GLBackend::dispatch(UploadNUniforms* data)
{
  UploadNUniforms* uniform_command = data;
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
     glUniformMatrix4fv(uniform_command->location, uniform_command->n, uniform_command->transpose, (const GLfloat*) uniform_data);
     break;
  }
}
  
void GLBackend::dispatch(DrawIndexed* data)
{
  DrawIndexed* command = data;

  bind_vao(command->vao);
  bind_material(command->material);
    
  glDrawElements(command->draw_type, command->num_indices, command->indices_type, (const void*) (get_type_size(command->indices_type) * command->start_index));
}
  
void GLBackend::dispatch(DrawIndexedInstanced* data)
{
  DrawIndexedInstanced* command = data;
    
  bind_vao(command->vao);
  bind_material(command->material);
    
  glDrawElementsInstanced(command->draw_type, command->num_indices, command->indices_type, (const void*) (get_type_size(command->indices_type) * command->start_index), command->instances);
}
  
}
}
