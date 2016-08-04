#version 150 core
in vec4 vertColor; // The input variable from the vertex shader (same name and same type)
  
out vec4 color;

void main()
{
  color = vertColor;
} 