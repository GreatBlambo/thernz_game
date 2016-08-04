#include <string.h>
#include <algorithm>

#include "rendering.h"

////////////////////////////////////////////////////////////////////////////////
// Command buffer
////////////////////////////////////////////////////////////////////////////////

GameError create_command_buffer(CommandBuffer* command_buffer, Buffer* buffer, size_t max_commands, size_t size)
{
  GameError err = NO_ERROR;

  if (!command_buffer || !buffer)
    return ERROR_NULL_PARAM;

  if (err = push_buffer(buffer, &command_buffer->command_data_buffer, size + (sizeof(CommandData) * max_commands))) return err;
  if (!push_array<CommandData*>(buffer, max_commands)) return ERROR_BUFFER_SIZE;
  
  command_buffer->num_commands = 0;
  command_buffer->max_commands = max_commands;
  
  return err;
}

/**
 * command_buffer_reserve_space: reserves space in the command_buffer's thread local data buffer for adding commands.
 * Note that this limits threads to one command buffer at a time.
 */
GameError command_buffer_reserve_space(CommandBuffer* command_buffer, Buffer* reserved_buffer, size_t size)
{
  if (!command_buffer || !reserved_buffer)
    return ERROR_NULL_PARAM;
  GameError err = NO_ERROR;

  SDL_AtomicLock(&command_buffer->buffer_lock);
  if (err = push_buffer(&command_buffer->command_data_buffer, reserved_buffer, size)) return err;
  SDL_AtomicUnlock(&command_buffer->buffer_lock);
  
  return err;
}

struct CommandBufferSortComp
{
  bool operator() (const CommandData* x, const CommandData* y)
  {
    return (x->key < y->key);
  }
} command_buffer_sort_compare;

GameError command_buffer_sort(CommandBuffer* command_buffer)
{
  if (!command_buffer)
    return ERROR_NULL_PARAM;

  std::sort(command_buffer->commands, command_buffer->commands + command_buffer->num_commands, command_buffer_sort_compare);

  return NO_ERROR;
}

GameError command_buffer_push(CommandBuffer* command_buffer
                              , DispatchFunction dispatch_function
                              , uint64_t key
                              , void* data
                              , size_t size)
{
  if (!command_buffer || !dispatch_function)
    return ERROR_NULL_PARAM;
  
  if (command_buffer->num_commands + 1 > command_buffer->max_commands)
    return ERROR_ARRAY_SIZE;
  
  CommandData* new_command = *(command_buffer->commands + command_buffer->num_commands);

  SDL_AtomicLock(&command_buffer->buffer_lock);
  command_buffer->num_commands++;
  SDL_AtomicUnlock(&command_buffer->buffer_lock);

  // allocate new command on thread local data buffer
  new_command->data = data;
  new_command->size = size;
  new_command->dispatch_function = dispatch_function;
  
  return NO_ERROR;
}

GameError command_buffer_reset(CommandBuffer* command_buffer)
{
  if (!command_buffer)
    return ERROR_NULL_PARAM;
  command_buffer->num_commands = 0;
  return NO_ERROR;
}

GameError render(CommandBuffer* command_buffer)
{
  if (!command_buffer)
    return ERROR_NULL_PARAM;
  
  GameError err = NO_ERROR;
  if (err = command_buffer_sort(command_buffer)) return err;

  for (size_t i = 0; i < command_buffer->num_commands; i++)
  {
    CommandData* command = command_buffer->commands[i];
    if (!command || !command->data || !command->dispatch_function)
      continue;
    command->dispatch_function(command->data);
  }
  command_buffer_reset(command_buffer);
  
  return err;
}

////////////////////////////////////////////////////////////////////////////////
// Sprite batch
////////////////////////////////////////////////////////////////////////////////

struct SpriteCommandData
{
};

GameError create_sprite_batch(SpriteBatch* sprite_batch, Buffer* buffer, size_t max_sprites)
{
  GameError err = NO_ERROR;
  if (err = create_command_buffer(&sprite_batch->command_buffer, buffer,
                                  max_sprites, max_sprites * sizeof(SpriteCommandData))) return err;
  return err;
}

GameError render(SpriteBatch* sprite_batch)
{
  render(&sprite_batch->command_buffer);
}

GameError submit_draw_sprite(SpriteBatch* sprite_batch, Sprite* sprite, glm::vec2 position, glm::vec2 size, float rotate)
{
}

GameError submit_draw_sprite(SpriteBatch* sprite_batch, Sprite* sprite, glm::mat4 model)
{
}
