#pragma once

#include <SDL2/SDL_atomic.h>
#include <glm/glm.hpp>

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
  // each thread gets a buffer to block out a certain amount of memory for command data
  Buffer command_data_buffer;
  CommandData** commands;
  size_t num_commands;
  size_t max_commands;

  SDL_SpinLock buffer_lock = 0;
};

GameError create_command_buffer(CommandBuffer* command_buffer, Buffer* buffer, size_t max_commands, size_t size);
GameError command_buffer_reserve_space(CommandBuffer* command_buffer, Buffer* reserved_buffer, size_t size);
GameError command_buffer_sort(CommandBuffer* command_buffer);
GameError command_buffer_push(CommandBuffer* command_buffer                              
                              , DispatchFunction dispatch_function
                              , uint64_t key
                              , void* data
                              , size_t size);
GameError command_buffer_reset(CommandBuffer* command_buffer);

////////////////////////////////////////////////////////////////////////////////
// Sprite renderer
////////////////////////////////////////////////////////////////////////////////

struct SpriteBatch
{
  CommandBuffer command_buffer;
  TextureID current_texture;
  ShaderProgramID current_shader;
};

GameError create_sprite_batch(SpriteBatch* sprite_batch, Buffer* buffer, size_t max_sprites);
GameError render(SpriteBatch* sprite_batch);
GameError submit_draw_sprite(SpriteBatch* sprite_batch, Sprite* sprite, glm::vec2 position, glm::vec2 size, float rotate);
GameError submit_draw_sprite(SpriteBatch* sprite_batch, Sprite* sprite, glm::mat4 model);
