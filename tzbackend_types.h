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
    class BackendGL : Backend
    {     
    public:
      BackendGL(size_t reserve_ids, foundation::Allocator& allocator)
        : m_main_window(0)
	, m_context(0)
	, m_gl_object_map(reserve_ids, allocator)
        , m_binding_map(reserve_ids, allocator)
        , m_allocator(allocator)
	{
	}
      
      void init();
      void deinit();

      void set_window_name(const char* name);
      void set_window_size(int width, int height);
      void set_window_fullscreen();
      void set_window_fullscreen_windowed();
      void set_window_windowed();
      void set_window_position(int x, int y);
      
      void swap_backbuffer();

      ShaderHandle load_shader(const char* pathname, ShaderType shader_type);
      void destroy_shader(ShaderHandle shader);

      ProgramHandle link_program(ShaderHandle* shaders, size_t num_shaders,
				 const VertexAttribArray& vert_spec);
      void destroy_program(ProgramHandle program);

      Texture load_texture(const char* pathname, int* width, int* height);
      void destroy_texture(Texture& texture);

      BufferHandle create_buffer(void* data, size_t data_size, BufferUsage usage);
      void update_buffer(BufferHandle handle, size_t offset, void* data, size_t data_size);
      void destroy_buffer(BufferHandle handle);
      
      BindingHandle create_binding(const VertexAttribArray& vert_spec);
      void destroy_binding(BindingHandle binding);
      
      // Command dispatch
      void dispatch(SetPipelineState* command);
      void dispatch(SetBuffer* command);
      void dispatch(UploadNUniforms* command);
      void dispatch(DrawIndexed* command);
      void dispatch(ClearBackbuffer* command);

    private:
      HandleMap<ResourceHandle, GLuint> m_gl_object_map;
      HandleMap<ResourceHandle, VertexLayout> m_binding_map;

      foundation::Allocator& m_allocator;

      GLuint m_vao; // dummy vao
            
      SDL_Window* m_main_window;
      SDL_GLContext m_context;
    };
  }
}
