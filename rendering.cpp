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
      
  if (err = push_buffer(buffer, &command_buffer->command_data_buffer + (sizeof(CommandData) * max_commands), size)) return err;
  if (!push_array<CommandData*>(buffer, max_commands)) return ERROR_BUFFER_SIZE;
  
  command_buffer->num_commands = 0;
  command_buffer->max_commands = max_commands;
  
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

  CommandData* new_command;
  if (!(new_command = (CommandData*) push_size(&(command_buffer->command_data_buffer), sizeof(CommandData)))) return ERROR_BUFFER_SIZE;  
  if (!(new_command->data = push_size(&(command_buffer->command_data_buffer), size))) return ERROR_BUFFER_SIZE;
  new_command->size = size;
  new_command->dispatch_function = dispatch_function;

  memcpy(new_command->data, data, size);

  command_buffer->commands[command_buffer->num_commands] = new_command;
  
  command_buffer->num_commands++;
  return NO_ERROR;
}

GameError command_buffer_reset(CommandBuffer* command_buffer)
{
  if (!command_buffer)
    return ERROR_NULL_PARAM;
  command_buffer->command_data_buffer;
  command_buffer->num_commands = 0;
  return NO_ERROR;
}

////////////////////////////////////////////////////////////////////////////////
// Renderer
////////////////////////////////////////////////////////////////////////////////

GameError create_renderer(Renderer* renderer, Buffer* buffer, size_t max_commands, size_t size)
{
  if (!renderer || ! buffer)
    return ERROR_NULL_PARAM;
  
  GameError err = NO_ERROR;
  if (err = create_command_buffer(&renderer->command_buffer, buffer, max_commands, size)) return err;
  
  return err;
}

GameError render(Renderer* renderer)
{
  if (!renderer)
    return ERROR_NULL_PARAM;
  
  GameError err = NO_ERROR;
  CommandBuffer* command_buffer = &renderer->command_buffer;

  CHECK_ERR_RETURN(err, command_buffer_sort(command_buffer));

  for (size_t i = 0; i < command_buffer->num_commands; i++)
  {
    CommandData* command = command_buffer->commands[i];
    if (!command || !command->data || !command->dispatch_function)
      continue;
    command->dispatch_function(command->data);
  }

  return err;
}
