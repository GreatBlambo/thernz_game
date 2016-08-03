#pragma once
#include <SDL2/SDL.h>
#include <GL/glew.h>

struct Color
{
  float r;
  float g;
  float b;
  float a;
};

typedef GLuint TextureID;
typedef GLuint VertexBufferID;
typedef GLuint VertexArrayID;
typedef GLuint IndexArrayID;

struct Sprite
{
  TextureID texture;
  SDL_Rect rect;
};
