#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "tzbackend_dispatch.h"
#include "tzbackend_types.h"
#include "tzrender_types_gl.h"

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

////////////////////////////////////////////////////////////////////////////////
// Backend commands
////////////////////////////////////////////////////////////////////////////////

TZ_BACKEND_DISPATCH_API_IMPL(BackendGL, UploadNUniforms)
{
  UploadNUniforms* uniform_command = (UploadNUniforms*) data;
  void* uniform_data = Command::get_aux_data(Command::get_command(data));
    
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

TZ_BACKEND_DISPATCH_API_IMPL(BackendGL, DrawIndexed)
{
  DrawIndexed* command = (DrawIndexed*) data;

  bind_vao(command->vao);
  bind_material(command->material);

  if (command->instances == 0)
  {
    glDrawElements(get_draw_type(command->draw_type),
                   command->num_indices,
                   command->indices_type,
                   (const void*) (get_type_size(command->indices_type) * command->start_index));
  }
  else
  {
    glDrawElementsInstanced(get_draw_type(command->draw_type),
                          command->num_indices,
                          command->indices_type,
                          (const void*) (get_type_size(command->indices_type) * command->start_index),
                          command->instances);
  }
}

TZ_BACKEND_FREE_FUNCTION_IMPL(BackendGL, void, clear_backbuffer, GraphicsBitfield bitfield)
{
  glClear(translate_bitfield(bitfield));
}

}
}
