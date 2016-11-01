#pragma once

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "tzbackend_dispatch.h"
#include "tzhandle.h"

namespace tz
{
  namespace graphics
  {  
    TZ_HANDLE(ResourceHandle, uint32_t, 21);
    struct ShaderHandle  { ResourceHandle id; };
    struct ProgramHandle { ResourceHandle id; };
    struct TextureHandle { ResourceHandle id; };
    struct BindingHandle { ResourceHandle id; };
      
    class Backend
    {
    public:
      typedef HandleSet<ResourceHandle> ResourceHandleSet;
      
      virtual void init() = 0;
      virtual void deinit() = 0;
        
      virtual void set_window_name(const char* name) = 0;
      virtual void set_window_size(int width, int height) = 0;
      virtual void set_window_fullscreen() = 0;
      virtual void set_window_fullscreen_windowed() = 0;
      virtual void set_window_windowed() = 0;
      
      virtual void swap_backbuffer() = 0;

      virtual ShaderHandle load_shader(const char* pathname, ShaderType shader_type) = 0;
      virtual void destroy_shader(ShaderHandle shader) = 0;

      virtual ProgramHandle link_program(ShaderHandle* shaders, size_t num_shaders,
					 const VertexAttribArray& vert_spec) = 0;
      virtual void destroy_program(ProgramHandle program) = 0;

      virtual TextureHandle load_texture(const char* pathname) = 0;
      virtual void destroy_texture(TextureHandle texture) = 0;
      
      virtual BindingHandle create_indexed_binding(const VertexAttribArray& vert_spec) = 0;
      virtual void destroy_binding(BindingHandle binding) = 0;
    };
    
    
    class BackendGL : Backend
    {
    public:
      BackendGL(foundation::Allocator& allocator)
        : m_main_window(0)
	, m_context(0)
	, m_gl_object_ids(allocator)
	{}
      void init();
      void deinit();
        
      void set_window_name(const char* name);
      void set_window_size(int width, int height);
      void set_window_fullscreen();
      void set_window_fullscreen_windowed();
      void set_window_windowed();
      
      void swap_backbuffer();

      ShaderHandle load_shader(const char* pathname, ShaderType shader_type);
      void destroy_shader(ShaderHandle shader);

      ProgramHandle link_program(ShaderHandle* shaders, size_t num_shaders,
				 const VertexAttribArray& vert_spec);
      void destroy_program(ProgramHandle program);

      TextureHandle load_texture(const char* pathname);
      void destroy_texture(TextureHandle texture);
      
      BindingHandle create_indexed_binding(const VertexAttribArray& vert_spec);
      void destroy_binding(BindingHandle binding);

    private:
      Backend::ResourceHandleSet* m_resource_handles;
      foundation::Array<GLuint> m_gl_object_ids;
      
      SDL_Window* m_main_window;
      SDL_GLContext m_context;
    };
  }
  
  namespace renderer
  {
    TZ_GRAPHICS_BACKEND(BackendGL);
  }
}
