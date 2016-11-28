#pragma once
#include "tzrender_types.h"

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

namespace tz
{
  namespace renderer
  {
    inline GLenum get_buffer_type(BufferType buffer_type)
    {
      switch(buffer_type)
      {
      case VERTEX_BUFFER: return GL_ARRAY_BUFFER; break;
      case INDEX_BUFFER: return GL_ELEMENT_ARRAY_BUFFER; break;
      };
    }
    
    inline GLenum get_buffer_usage(BufferUsage buffer_usage)
    {
      switch (buffer_usage)
      {
      case STREAM_DRAW: return GL_STREAM_DRAW ; break;
      case STREAM_READ: return GL_STREAM_READ ; break;
      case STREAM_COPY: return GL_STREAM_COPY ; break;
      case STATIC_DRAW: return GL_STATIC_DRAW ; break;
      case STATIC_READ: return GL_STATIC_READ ; break;
      case STATIC_COPY: return GL_STATIC_COPY ; break;
      case DYNAMIC_DRAW: return GL_DYNAMIC_DRAW ; break;
      case DYNAMIC_READ: return GL_DYNAMIC_READ ; break;
      case DYNAMIC_COPY: return GL_DYNAMIC_COPY ; break;
      }
    }
  
    inline GLenum get_draw_type(DrawType draw_type)
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

    inline GLenum get_texture_type(TextureType texture_type)
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

    inline GLenum get_shader_type(ShaderType shader_type)
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

    inline GLenum get_data_type(DataType data_type)
    {
      switch (data_type)
	{
	case UNSIGNED_BYTE: return GL_UNSIGNED_BYTE; break;
	case UNSIGNED_SHORT: return GL_UNSIGNED_SHORT; break;
	case UNSIGNED_INT: return GL_UNSIGNED_INT; break;
	case FLOAT: return GL_FLOAT; break;
	}
    };

    inline GLbitfield translate_bitfield(GraphicsBitfield bitfield)
    {
      GLbitfield result = 0;
      if (bitfield & COLOR_BUFFER_BIT) result |= GL_COLOR_BUFFER_BIT;
      if (bitfield & DEPTH_BUFFER_BIT) result |= GL_DEPTH_BUFFER_BIT;
      if (bitfield & ACCUM_BUFFER_BIT) result |= GL_ACCUM_BUFFER_BIT;
      if (bitfield & STENCIL_BUFFER_BIT) result |= GL_STENCIL_BUFFER_BIT;
      return result;
    }

  }
}
