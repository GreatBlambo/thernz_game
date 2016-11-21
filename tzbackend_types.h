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
    class Backend
    {
    public:
      struct PipelineState
      {
	// Shaders
        ShaderHandle vertex_shader;
	ShaderHandle tess_control_shader;
	ShaderHandle tess_eval_shader;
	ShaderHandle geometry_shader;
	ShaderHandle fragment_shader;
	// Per-sample stuff
	
      };
      
      virtual void init() = 0;
      virtual void deinit() = 0;
        
      virtual void set_window_name(const char* name) = 0;
      virtual void set_window_size(int width, int height) = 0;
      virtual void set_window_fullscreen() = 0;
      virtual void set_window_fullscreen_windowed() = 0;
      virtual void set_window_windowed() = 0;
      virtual void set_window_position(int x, int y) = 0;
      
      virtual void swap_backbuffer() = 0;

      virtual ShaderHandle load_shader(const char* pathname, ShaderType shader_type) = 0;
      virtual void destroy_shader(ShaderHandle shader) = 0;

      virtual ProgramHandle link_program(ShaderHandle* shaders, size_t num_shaders,
					 const VertexAttribArray& vert_spec) = 0;
      virtual void destroy_program(ProgramHandle program) = 0;

      virtual Texture load_texture(const char* pathname, int* width, int* height) = 0;
      virtual void destroy_texture(Texture& texture) = 0;
      
      virtual BufferHandle create_buffer(void* data, size_t data_size, BufferUsage usage) = 0;
      virtual void update_buffer(BufferHandle handle, size_t offset, void* data, size_t data_size) = 0;
      virtual void destroy_buffer(BufferHandle handle) = 0;
      
      virtual BindingHandle create_binding(const VertexAttribArray& vert_spec) = 0;
      virtual void destroy_binding(BindingHandle handle) = 0;

      // Command dispatch
      virtual void dispatch(UploadNUniforms* command) = 0;
      virtual void dispatch(DrawIndexed* command) = 0;
      virtual void dispatch(ClearBackbuffer* command) = 0;
    };    
    
    class BackendGL : Backend
    {
      struct Buffer
      {
	size_t offset;
	size_t size;
      };
      
    public:
      BackendGL(size_t reserve_ids, foundation::Allocator& allocator)
        : m_main_window(0)
	, m_context(0)
	, m_gl_object_ids(allocator)
	, m_resource_handles(reserve_ids, allocator)
	{
	  foundation::array::reserve(m_gl_object_ids, reserve_ids);
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
      void dispatch(UploadNUniforms* command);
      void dispatch(DrawIndexed* command);
      void dispatch(ClearBackbuffer* command);

    private:
      ResourceHandle push_id(GLuint id);
      inline GLuint get_id(ResourceHandle handle)
      {
	return m_gl_object_ids[handle.index()];
      }

      void bind_material(Material& material);
      void bind_vao(BindingHandle binding);
      
      HandleSet<ResourceHandle> m_resource_handles;
      foundation::Array<GLuint> m_gl_object_ids;

      // We partition areas of big existing buffers
      GLuint data_buffer;
      GLuint index_buffer;
      
      SDL_Window* m_main_window;
      SDL_GLContext m_context;
    };
  }
}
