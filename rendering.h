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
  uint64_t key;
};

struct CommandBuffer
{
  Buffer command_data_buffer;
  CommandData** commands;
  size_t num_commands;
  size_t max_commands;
};

GameError create_command_buffer(CommandBuffer* command_buffer, Buffer* buffer, size_t max_commands, size_t size);
GameError command_buffer_sort(CommandBuffer* command_buffer);
GameError command_buffer_push(CommandBuffer* command_buffer
                              , DispatchFunction dispatch_function
                              , uint64_t key
                              , void* data
                              , size_t size);
GameError command_buffer_reset(CommandBuffer* command_buffer);

////////////////////////////////////////////////////////////////////////////////
// Renderer
////////////////////////////////////////////////////////////////////////////////

struct Renderer
{
  CommandBuffer command_buffer;
  VertexArrayID current_vao;
  IndexArrayID current_ibo;
  TextureID current_texture;
};

GameError create_renderer(Renderer* renderer, Buffer* buffer, size_t max_commands, size_t size);
GameError render(Renderer* renderer);
GameError submit_draw_sprite(Renderer* renderer, Sprite* sprite);
