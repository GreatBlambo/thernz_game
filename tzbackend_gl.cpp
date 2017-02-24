#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "tzbackend_dispatch.h"
#include "tzbackend_types.h"
#include "tzrender_types_gl.h"

namespace tz
{
namespace graphics
{

////////////////////////////////////////////////////////////////////////////////
// BackendGL
////////////////////////////////////////////////////////////////////////////////

void BackendGL::init()
{
  // Init SDL video
  if (SDL_Init(SDL_INIT_VIDEO) < 0)
  {
    printf("SDL_Init Error: %s", SDL_GetError());
    return ERROR_SDL;
  }
  
  // Init SDL image
  int img_flags = IMG_INIT_PNG;
  if(!(IMG_Init(img_flags) & img_flags))
  {
    printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
    return ERROR_SDL;
  }
    
  m_main_window = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 0, 0, SDL_WINDOW_OPENGL);
  
  // Set our OpenGL version.
  // SDL_GL_CONTEXT_CORE gives us only the newer version, deprecated functions are disabled
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    
  // Turn on double buffering with a 24bit Z buffer.
  // You may need to change this to 16 or 32 for your system
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

  SDL_GL_SetSwapInterval(1);

  m_context = SDL_GL_CreateContext(m_main_window);
  if (!m_context)
  {
    printf("Failed to create OpenGL context: %s\n", SDL_GetError());
    return ERROR_OPENGL;
  }
  
  // Init GLEW
  glewExperimental = GL_TRUE;
  GLenum glew_err = glewInit();
  if (glew_err != GLEW_OK)
  {
    printf("Failed to initialize GLEW: %s\n", glewGetErrorString(glew_err));
    return ERROR_GLEW;
  }

  // Dummy vao
  m_vao = 0;
  glGenVertexArrays(1, &m_vao);
  glBindVertexArray(m_vao);
  
  return NO_ERROR;
}

void BackendGL::deinit()
{
  glDeleteVertexArrays(1, &vao);
  
  SDL_GL_DeleteContext(m_context);
  SDL_DestroyWindow(m_main_window);
  SDL_Quit();
}  

void BackendGL::set_window_name(const char* name)
{
  SDL_SetWindowTitle(m_main_window, name);
}

void BackendGL::set_window_size(int width, int height)
{
  SDL_SetWindowSize(m_main_window, width, height);
  SDL_SetWindowPosition(m_main_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
  glViewport(0, 0, width, height);
}

void BackendGL::set_window_fullscreen()
{
  SDL_SetWindowFullscreen(m_main_window, SDL_WINDOW_FULLSCREEN);
}

void BackendGL::set_window_fullscreen_windowed()
{
  SDL_SetWindowFullscreen(m_main_window, SDL_WINDOW_FULLSCREEN_DESKTOP);
}

void BackendGL::set_window_windowed()
{
  SDL_SetWindowFullscreen(m_main_window, 0);
}

void BackendGL::set_window_position(int x, int y)
{
  SDL_SetWindowPosition(m_main_window, x, y);
}

void BackendGL::swap_backbuffer()
{
  SDL_GL_SwapWindow(m_main_window);
}

ShaderHandle BackendGL::load_shader(const char* pathname, ShaderType shader_type)
{
  static const size_t ERROR_BUFF_SIZE = 512;
  
  GLuint result;
  FILE* file = NULL;
  size_t file_size;
  char* shader_string_buffer;
  
  file = fopen(pathname, "r+");
  if (!file)
  {
    printf("Unable to load file %s.\n", pathname);
    return 0;
  }
  fseek(file, 0L, SEEK_END);
  file_size = ftell(file);
  rewind(file);

  shader_string_buffer = (char*) malloc(file_size + 1);
  if (fread(shader_string_buffer, 1, file_size, file) != file_size)
  {
    printf("Read error when reading %s.\n", pathname);
    return 0;
  }
  shader_string_buffer[file_size] = 0;

  result = glCreateShader(renderer::get_shader_type(shader_type));
  glShaderSource(result, 1, &shader_string_buffer, (int*) &file_size);
  glCompileShader(result);

  int is_compiled;
  glGetShaderiv(result, GL_COMPILE_STATUS, (int*) &is_compiled);

  if (!(is_compiled == GL_TRUE))
  {
    char error_buff[ERROR_BUFF_SIZE];
    int max_length = 0;
    glGetShaderiv(result, GL_INFO_LOG_LENGTH, &max_length);
    glGetShaderInfoLog(result, max_length, &max_length, error_buff);

    error_buff[max_length] = 0;
    
    glDeleteShader(result);
    result = 0;
    TZ_ASSERT(false, "Compile issue: %s", error_buff);
  }

  free(shader_string_buffer);
  fclose(file);
  
  return { m_gl_objects_map.add(result) };

}

void BackendGL::destroy_shader(ShaderHandle shader)
{
  if (!m_gl_objects_map.has(shader.id)) return;
  glDeleteShader(m_gl_objects_map.get(shader.id));
  m_gl_objects_map.remove(shader.id);
}

ProgramHandle BackendGL::link_program(ShaderHandle* shaders, size_t num_shaders,
                                      const VertexAttribArray& vert_spec)
{
  GLuint program = glCreateProgram();
  for (size_t i = 0; i < num_shaders; i++)
  {
    glAttachShader(program, get_id(shaders[i].id));
  }
  for (size_t i = 0; i < foundation::array::size(vertex_spec); i++)
  {
    const VertexAttribute& attrib = vertex_spec[i];
    glBindAttribLocation(program,
                         attrib.location, attrib.name);
  }

  glLinkProgram(program);
  
  int is_linked;
  glGetProgramiv(program, GL_LINK_STATUS, &is_linked);

  if (!is_linked)
  {
    char error_buff[ERROR_BUFF_SIZE];
    int max_length = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &max_length);
    glGetProgramInfoLog(program, max_length, &max_length, error_buff);

    error_buff[max_length] = 0;    
    glDeleteProgram(program);
    TZ_ASSERT(false, "Shader linking error: %s", error_buff);
  }
  
  return { m_gl_objects_map.add(program) };
}

void BackendGL::destroy_program(ProgramHandle program)
{
  if (!m_gl_objects_map.has(program.id)) return;
  glDeleteProgram(m_gl_objects_map.get(program.id));
  m_gl_objects_map.remove(program.id);
}

Texture BackendGL::load_texture(const char* pathname, int* width, int* height)
{  
  //Load image at specified path
  SDL_Surface* loaded_surface = IMG_Load(pathname);
  if(loaded_surface == NULL)
  {
    TZ_ASSERT(false, "Unable to load image %s! SDL_image Error: %s\n", pathname, IMG_GetError());
  }
  //Create texture from surface pixels
  GLuint texture_id;
  glGenTextures(1, &texture_id);
  glBindTexture(GL_TEXTURE_2D, texture_id);
  
  int tex_mode = GL_RGB;
  
  if(loaded_surface->format->BytesPerPixel == 4) {
    tex_mode = GL_RGBA;
  }
  
  glTexImage2D(GL_TEXTURE_2D, 0, tex_mode, loaded_surface->w, loaded_surface->h, 0, tex_mode, GL_UNSIGNED_BYTE, loaded_surface->pixels);
  
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  ResourceHandle handle = push_id(texture_id);
  Texture texture;
  texture->texture_id = { handle };
  texture->texture_type = TEXTURE_2D;
  *width = loaded_surface->w;
  *height = loaded_surface->h;
  
  //Clean up surface
  SDL_FreeSurface(loaded_surface);
  
  return texture;
}

void BackendGL::destroy_texture(Texture& texture)
{
  if (!m_gl_objects_map.has(texture.texture_id.id)) return;
  Gluint tex = m_gl_objects_map.get(texture.texture_id.id);
  glDeleteTextures(1, &tex);

  m_gl_objects_map.remove(texture.texture_id.id);
}

BufferHandle BackendGL::create_buffer(void* data, size_t data_size, BufferUsage usage)
{
  // create vbo
  GLuint vbo;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, data_size, data, renderer::get_buffer_usage(usage));
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  return { m_gl_object_map.add(vbo) };
}

void BackendGL::destroy_buffer(BufferHandle handle)
{
  m_gl_object_map.remove(handle.id);
}
      
BindingHandle BackendGL::create_binding(const VertexLayout& vert_spec)
{
  // Copy vert_spec
  VertexLayout new_vert_spec(vert_spec, m_allocator);
  return { m_binding_map.add(new_vert_spec) };
}

BindingHandle BackendGL::create_binding(const VertexAttribute* vert_attribs, size_t num_attribs)
{
  VertexLayout new_vert_spec(vert_attribs, num_attribs, m_allocator);
  return { m_binding_map.add(new_vert_spec) };
}

void BackendGL::destroy_binding(BindingHandle binding)
{
  m_binding_map.remove(binding.id);
}

////////////////////////////////////////////////////////////////////////////////
// Backend commands
////////////////////////////////////////////////////////////////////////////////

void BackendGL::dispatch(SetPipelineState* command)
{
  // Set per sample stuff

  // Bind vertex arrays, set shader program
  
  // bind vertex arrays
  if (!m_binding_map.has(command->vertex_format))
    return; // Vertex format was not registered. Abort

  VertexLayout& vert_spec = m_binding_map.get(command->vertex_format);
  for (size_t i = 0; i < vert_spec.num_attribs; i++)
  {
    VertexAttribute& attrib = vert_spec[i];
    glEnableVertexAttribArray(attrib.location);
    glVertexAttribPointer(attrib.location, attrib.size,
                          renderer::get_data_type(attrib.type),
                          false,
                          vert_spec.vert_size, (const void*) attrib.offset);
  }
  
  // set shader
  glUseProgram(m_gl_object_map(command->shader_stages.id));
}

void BackendGL::dispatch(SetBuffer* command)
{
  
}

void BackendGL::dispatch(UploadNUniforms* uniform_command)
{
  void* uniform_data = Command::get_aux_data(Command::get_command((void*) uniform_comand));
    
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

void BackendGL::dispatch(DrawIndexed* command);
{
  // deal with offsets somehow
  if (command->instances > 1)
  {
    glDrawElementsInstanced(get_draw_type(command->draw_type),
                            command->num_indices,
                            get_data_type(command->indices_type),
                            (const void*) (get_type_size(command->indices_type) * command->start_index),
                            command->instances);
  }
  else if (command->instances == 1)
  {
    glDrawElements(get_draw_type(command->draw_type),
                   command->num_indices,
                   get_data_type(command->indices_type),
                   (const void*) (get_type_size(command->indices_type) * command->start_index));
  }
}

void BackendGL::dispatch(ClearBackbuffer* command)
{
  glClear(translate_bitfield(command->bitfield));
}

}
}
