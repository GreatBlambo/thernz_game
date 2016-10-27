#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "tzbackend_dispatch.h"
#include "tzbackend_types.h"

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

inline static GLenum get_draw_type(DrawType draw_type)
{
  switch (draw_type)
  {
   case POINTS: return GL_POINTS; break;
   case LINES: return GL_LINES; break;
   case LINE_LOOP: return GL_LINE_LOOP; break;
   case LINE_STRIP: return GL_LINE_STRIP; break;
   case TRIANGLES: return GL_TRIANGLES; break;
   case TRIANGLE_STRIP: return GL_TRIANGLE_STRIP; break;
   case TRIANGLE_FAN: return GL_TRIANGLE_FAN; break;
  }
};

inline static GLenum get_texture_type(TextureType texture_type)
{
  switch (texture_type)
  {
   case TEXTURE_1D: return GL_TEXTURE_1D; break;
   case TEXTURE_2D: return GL_TEXTURE_2D; break;
   case TEXTURE_3D: return GL_TEXTURE_3D; break;
   case TEXTURE_1D_ARRAY: return GL_TEXTURE_1D_ARRAY; break;
   case TEXTURE_2D_ARRAY: return GL_TEXTURE_2D_ARRAY; break;
   case TEXTURE_RECTANGLE: return GL_TEXTURE_RECTANGLE; break;
   case TEXTURE_CUBE_MAP: return GL_TEXTURE_CUBE_MAP; break;
   case TEXTURE_CUBE_MAP_ARRAY: return GL_TEXTURE_CUBE_MAP_ARRAY; break;
   case TEXTURE_BUFFER: return GL_TEXTURE_BUFFER; break;
   case TEXTURE_2D_MULTISAMPLE: return GL_TEXTURE_2D_MULTISAMPLE; break;
   case TEXTURE_2D_MULTISAMPLE_ARRAY: return GL_TEXTURE_2D_MULTISAMPLE_ARRAY; break;
  }
};

inline static GLenum get_shader_type(ShaderType shader_type)
{
  switch (shader_type)
  {
   case VERTEX_SHADER: return GL_VERTEX_SHADER; break;
   case TESS_CONTROL_SHADER: return GL_TESS_CONTROL_SHADER; break;
   case TESS_EVALUATION_SHADER: return GL_TESS_EVALUATION_SHADER; break;
   case GEOMETRY_SHADER: return GL_GEOMETRY_SHADER; break;
   case FRAGMENT_SHADER: return GL_FRAGMENT_SHADER; break;
   case COMPUTE_SHADER: return GL_COMPUTE_SHADER; break;
  }
};

inline static GLenum get_data_type(DataType data_type)
{
  switch (data_type)
  {
   case UNSIGNED_BYTE: return GL_UNSIGNED_BYTE; break;
   case UNSIGNED_SHORT: return GL_UNSIGNED_SHORT; break;
   case UNSIGNED_INT: return GL_UNSIGNED_INT; break;
   case FLOAT: return GL_FLOAT; break;
  }
};

inline static GLbitfield translate_bitfield(GraphicsBitfield bitfield)
{
  GLbitfield result = 0;
  if (bitfield & COLOR_BUFFER_BIT) result |= GL_COLOR_BUFFER_BIT;
  if (bitfield & DEPTH_BUFFER_BIT) result |= GL_DEPTH_BUFFER_BIT;
  if (bitfield & ACCUM_BUFFER_BIT) result |= GL_ACCUM_BUFFER_BIT;
  if (bitfield & STENCIL_BUFFER_BIT) result |= GL_STENCIL_BUFFER_BIT;
  return result;
}

////////////////////////////////////////////////////////////////////////////////
// Backend functions
////////////////////////////////////////////////////////////////////////////////

BackendGL::BackendGL()
  : m_main_window(NULL)
  , m_context(0)
{}

GameError BackendGL::init()
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
    
  m_main_window = SDL_CreateWindow("",
                                   SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                   800, 600,
                                   SDL_WINDOW_OPENGL);

  m_context = SDL_GL_CreateContext(m_main_window);
  
  if (!m_context)
  {
    printf("Failed to create OpenGL context: %s\n", SDL_GetError());
    return ERROR_OPENGL;
  }

  // Set our OpenGL version.
  // SDL_GL_CONTEXT_CORE gives us only the newer version, deprecated functions are disabled
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    
  // Turn on double buffering with a 24bit Z buffer.
  // You may need to change this to 16 or 32 for your system
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

  SDL_GL_SetSwapInterval(1);
  
  // Init GLEW
  glewExperimental = GL_TRUE;
  GLenum glew_err = glewInit();
  if (glew_err != GLEW_OK)
  {
    printf("Failed to initialize GLEW: %s\n", glewGetErrorString(glew_err));
    return ERROR_GLEW;
  }

  return  NO_ERROR;
}

void BackendGL::deinit()
{
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

void BackendGL::clear_backbuffer(GraphicsBitfield bitfield)
{
  glClear(translate_bitfield(bitfield));
}

GameError BackendGL::load_image_as_texture(int* width, int* height, Texture* texture, const char* pathname)
{
  //Load image at specified path
  SDL_Surface* loaded_surface = IMG_Load(pathname);
  if(loaded_surface == NULL)
  {
    printf("Unable to load image %s! SDL_image Error: %s\n", pathname, IMG_GetError());
    return ERROR_SDL;
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

  texture->texture_id = texture_id;
  *width = loaded_surface->w;
  *height = loaded_surface->h;
  
  //Clean up surface
  SDL_FreeSurface(loaded_surface);
  
  return NO_ERROR;
}

void BackendGL::destroy_texture(Texture* texture)
{
  glDeleteTextures(1, &texture->texture_id);
}

// Shaders
#define ERROR_BUFF_SIZE 512

ShaderID BackendGL::load_shader_source(const char* pathname, ShaderType shader_type)
{
  ShaderID result;
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

  result = glCreateShader(shader_type);
  glShaderSource(result, 1, &shader_string_buffer, (int*) &file_size);
  glCompileShader(result);

  int is_compiled;
  glGetShaderiv(result, GL_COMPILE_STATUS, (int*) &is_compiled);

  if (!is_compiled)
  {
    printf("Compile issue:");
    char error_buff[ERROR_BUFF_SIZE];
    int max_length = 0;
    glGetShaderiv(result, GL_INFO_LOG_LENGTH, &max_length);
    glGetShaderInfoLog(result, max_length, &max_length, error_buff);

    error_buff[max_length] = 0;
    printf("%s\n", error_buff);
    
    glDeleteShader(result);
    result = 0;
  }

  free(shader_string_buffer);
  fclose(file);
  return result;
}

void BackendGL::destroy_shader(ShaderID shader)
{
  glDeleteShader(shader);
}

ShaderProgramID BackendGL::link_shader_program(ShaderID* shaders, size_t num_shaders, const VertexAttribArray& vertex_spec)
{
  if (!shaders)
    return 0;
  
  ShaderProgramID program = glCreateProgram();
  for (size_t i = 0; i < num_shaders; i++)
  {
    glAttachShader(program, shaders[i]);
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
    printf("Linking error\n");

    char error_buff[ERROR_BUFF_SIZE];
    int max_length = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &max_length);
    glGetProgramInfoLog(program, max_length, &max_length, error_buff);

    error_buff[max_length] = 0;
    printf("%s\n", error_buff);
    
    glDeleteProgram(program);
    return 0;
  }
  
  return program;
}

void BackendGL::destroy_program(ShaderProgramID program)
{
  glDeleteProgram(program);
}

GameError BackendGL::detach_shaders(ShaderProgramID program, ShaderID* shaders, size_t num_shaders)
{
  if (!shaders)
    return ERROR_NULL_PARAM;
  for (size_t i = 0; i < num_shaders; i++)
  {
    glDetachShader(program, shaders[i]);
  }
  return NO_ERROR;
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



}
}
