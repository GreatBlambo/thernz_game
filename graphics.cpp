#include "graphics.h"
#include "render_types.h"

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

////////////////////////////////////////////////////////////////////////////////
// Assets
////////////////////////////////////////////////////////////////////////////////

// Textures

GameError load_image_as_texture(Texture* texture, const char* pathname)
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
  texture->w = loaded_surface->w;
  texture->h = loaded_surface->h;
  
  //Clean up surface
  SDL_FreeSurface(loaded_surface);
  
  return NO_ERROR;
}

bool texture_is_valid(Texture* texture)
{
  return texture->texture_id > 0;
}

void destroy_texture(Texture* texture)
{
  glDeleteTextures(1, &texture->texture_id);
}

// Shaders
#define ERROR_BUFF_SIZE 512

ShaderID load_shader_source(const char* pathname, GLenum shader_type)
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

ShaderProgramID link_shader_program(ShaderID* shaders, size_t num_shaders, const VertSpec& vertex_spec)
{
  if (!shaders)
    return 0;
  
  ShaderProgramID program = glCreateProgram();
  for (size_t i = 0; i < num_shaders; i++)
  {
    glAttachShader(program, shaders[i]);
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

  for (size_t i = 0; i < vertex_spec.num_attributes; i++)
  {
    glBindAttribLocation(program,
                         vertex_spec.attrib_locations[i], vertex_spec.attrib_names[i]);
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

////////////////////////////////////////////////////////////////////////////////
// Windows and Graphics
////////////////////////////////////////////////////////////////////////////////

SDL_Window* create_window(const char* window_name
                          , float width, float height
                          , bool fullscreen
                          , float pos_x, float pos_y)
{
  const char* name = "";
  if (name)
    name = window_name;
  
  Uint32 flags = SDL_WINDOW_OPENGL;
  if (fullscreen)
    flags = flags | SDL_WINDOW_FULLSCREEN;
  
  return SDL_CreateWindow(name,
                          pos_x, pos_y,
                          width, height,
                          flags);
}

bool graphics_is_valid(Graphics* graphics)
{
  return (graphics && graphics->main_window && graphics->context);
}

GameError init_graphics(Graphics* graphics, WindowParams* window_params)
{
  // Sanitize params
  if (!graphics || !window_params)
    return ERROR_NULL_PARAM;
  
  // Load modules

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

  graphics->main_window = create_window(window_params->name
                                        , window_params->rect.w, window_params->rect.h
                                        , window_params->fullscreen
                                        , window_params->rect.x, window_params->rect.y);
  
  // Set our OpenGL version.
  // SDL_GL_CONTEXT_CORE gives us only the newer version, deprecated functions are disabled
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
 
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  
  // Turn on double buffering with a 24bit Z buffer.
  // You may need to change this to 16 or 32 for your system
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  
  graphics->context = SDL_GL_CreateContext(graphics->main_window);
  if (!graphics->context)
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

  glEnable(GL_BLEND);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  return NO_ERROR;
}

GameError destroy_graphics(Graphics* graphics)
{
  if (!graphics_is_valid(graphics))
    return ERROR_INVALID_PARAM;
  SDL_GL_DeleteContext(graphics->context);
  SDL_DestroyWindow(graphics->main_window);
  SDL_Quit();
  return NO_ERROR;
}
