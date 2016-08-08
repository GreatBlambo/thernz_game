#version 330 core

in vec2 frag_texcoords;
out vec4 color;

uniform sampler2D tex;
uniform vec3 sprite_color;

float alpha_threshold = 0.1;

void main()
{
  color = texture(tex, frag_texcoords) * vec4(sprite_color, 1.0);
  if (color.a < alpha_threshold)
  {
    discard;
  }
}
