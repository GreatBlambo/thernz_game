#version 330 core

in vec2 position;
in vec4 sprite_uv;
in mat4 model;

out vec2 frag_texcoords;

uniform mat4 projection;
uniform mat4 view;

vec2 uv_offset;

void main()
{
  uv_offset.x = position.x * sprite_uv.z;
  uv_offset.y = position.y * sprite_uv.w;
  frag_texcoords = sprite_uv.xy + uv_offset;
  gl_Position = projection * view * model * vec4(position, 0.0f, 1.0f);
}
