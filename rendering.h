#pragma once

#include "graphics.h"
#include "render_functions.h"
#include "render_types.h"
#include "memory.h"

////////////////////////////////////////////////////////////////////////////////
// Command buffer
////////////////////////////////////////////////////////////////////////////////

typedef void (*DispatchFunction)(void* data);

struct CommandData
{
  DispatchFunction dispatch_function;
  void* data;
  size_t size;
};

struct CommandBuffer
{
  Buffer command_data_buffer;
  uint64_t* keys;
  CommandData** commands;
  size_t num_commands;
};

GameError create_command_buffer(Buffer* buffer, CommandBuffer* command_buffer);
GameError sort_command_buffer(CommandBuffer* command_buffer);
GameError command_buffer_push(CommandBuffer* command_buffer
                              , DispatchFunction dispatch_function
                              , void* data
                              , size_t size);

////////////////////////////////////////////////////////////////////////////////
// Renderer
////////////////////////////////////////////////////////////////////////////////

struct Renderer
{
  CommandBuffer command_buffer;
  GLuint current_vao;
  GLuint current_ibo;
  TextureID current_texture;
};

GameError submit_draw_sprite(Renderer* renderer, Sprite* sprite);
