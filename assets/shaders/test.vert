#version 330 core

in vec2 vertex;
out vec2 frag_texcoords;

uniform mat4 model;
uniform mat4 projection;

void main()
{
  frag_texcoords = vertex;
  gl_Position = projection * model * vec4(vertex, 0.0f, 1.0f);
}
