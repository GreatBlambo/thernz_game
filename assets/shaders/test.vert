#version 330 core

in vec2 position;
out vec4 frag_color;

void main()
{
  gl_Position = position;
}

