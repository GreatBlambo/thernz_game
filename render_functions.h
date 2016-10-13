#include "graphics.h"
#include "render_types.h"

void clear_color(Color* color);

////////////////////////////////////////////////////////////////////////////////
// UBOs
////////////////////////////////////////////////////////////////////////////////

void shader_bind_ubo_block(ShaderProgramID program, const char* name, GLuint location);
UniformBufferID create_ubo_dynamic(size_t size, GLuint location);
UniformBufferID create_ubo_static(size_t size, GLuint location);
void delete_ubo(UniformBufferID ubo);
void upload_ubo_data(UniformBufferID ubo, size_t offset, size_t size, void* data);
