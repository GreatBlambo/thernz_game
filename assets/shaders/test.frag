#version 330 core

in vec4 sprite_color;
in vec2 frag_texcoords;
out vec4 color;

uniform sampler2D tex;

float alpha_threshold = 0.1;

void main()
{
  color = texture(tex, frag_texcoords) * sprite_color;
  if (color.a < alpha_threshold)
  {
    discard;
  }
}
