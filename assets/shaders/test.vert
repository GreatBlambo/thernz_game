#version 150 core
  
in vec3 position;
in vec3 normal;

out vec4 vertColor;
  
void main()
{
  gl_Position = vec4(position, 1.0);
  vertColor = vec4(normal, 1.0);
}