#include <glm/gtc/matrix_transform.hpp>
#include <SDL2/SDL_image.h>

#include "tzrender_types.h"

namespace tz
{

////////////////////////////////////////////////////////////////////////////////
// Types
////////////////////////////////////////////////////////////////////////////////

size_t get_type_size(DataType data_type)
{
  switch(data_type)
  {
   case(UNSIGNED_BYTE):
     return sizeof(GLubyte);
     break;
   case(UNSIGNED_SHORT):
     return sizeof(GLushort);
     break;
   case(UNSIGNED_INT):
     return sizeof(GLuint);
     break;
   case(FLOAT):
     return sizeof(GLfloat);
     break;
  }
}

////////////////////////////////////////////////////////////////////////////////
// Vertex Specification
////////////////////////////////////////////////////////////////////////////////

VertexAttribArray::VertexAttribArray(foundation::Allocator& alloc)
  : foundation::Array<VertexAttribute>(alloc)
{}

void push_attrib(VertexAttribArray& array, const char* name, int location, int size, DataType type)
{
  VertexAttribute new_attrib;
  strcpy(new_attrib.name, name);
  new_attrib.location = location;
  new_attrib.size = size;
  new_attrib.type = type;

  if (foundation::array::size(array) == 0)
  {
    new_attrib.offset = 0;
  }
  else
  {
    VertexAttribute& last = foundation::array::back(array);
    new_attrib.offset = last.offset + (last.size * get_type_size(last.type));
  }

  array.vert_size = new_attrib.offset + (size * get_type_size(type));

  foundation::array::push_back(array, new_attrib);
}

////////////////////////////////////////////////////////////////////////////////
// Sprites
////////////////////////////////////////////////////////////////////////////////

void create_sprite(Sprite* sprite, glm::vec2 texture_offset, glm::vec2 tex_dims, glm::vec2 sprite_dims, Texture* texture, Color color)
{
  sprite->sprite_uv.x = texture_offset.x / tex_dims.x;
  sprite->sprite_uv.y = texture_offset.y / tex_dims.y;
  sprite->sprite_uv.w = sprite_dims.x / tex_dims.x;
  sprite->sprite_uv.z = sprite_dims.y / tex_dims.y;
  sprite->color = glm::vec4(color, 1.0);
}

////////////////////////////////////////////////////////////////////////////////
// Assets
////////////////////////////////////////////////////////////////////////////////

// Textures

GameError load_image_as_texture(int* width, int* height, Texture* texture, const char* pathname)
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

void destroy_texture(Texture* texture)
{
  glDeleteTextures(1, &texture->texture_id);
}

// Shaders
#define ERROR_BUFF_SIZE 512

ShaderID load_shader_source(const char* pathname, ShaderType shader_type)
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

void destroy_shader(ShaderID shader)
{
  glDeleteShader(shader);
}

ShaderProgramID link_shader_program(ShaderID* shaders, size_t num_shaders, const VertexAttribArray& vertex_spec)
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

void destroy_program(ShaderProgramID program)
{
  glDeleteProgram(program);
}

GameError detach_shaders(ShaderProgramID program, ShaderID* shaders, size_t num_shaders)
{
  if (!shaders)
    return ERROR_NULL_PARAM;
  for (size_t i = 0; i < num_shaders; i++)
  {
    glDetachShader(program, shaders[i]);
  }
  return NO_ERROR;
}

}
