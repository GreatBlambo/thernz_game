#include "command_buffer.h"

GameError create_command_buffer(CommandBuffer* result, size_t max_commands,
				Buffer* perm_memory, FrameDataBuffer* frame_memory)
{
  // Check if frame_memory can support requested commands and if we can allocate enough memory for keys  
  size_t amount_required = NUM_BUFFERS * sizeof(Command) * max_commands;
  
  if (buffer_get_remaining(perm_memory) < amount_required)
    return ERROR_BUFFER_SIZE;
  
  result->current_buffer = 0;

  result->frame_memory = frame_memory;

  // Allocate the buffers
  for (size_t i = 0; i < NUM_BUFFERS; i++)
  {
    result->commands[i] = push_array<Command>(perm_memory, max_commands);
    result->num_commands[i] = 0;
  }
  result->max_commands = max_commands;

  RWLock_init(&result->rw_lock);

  return NO_ERROR;
}

GameError command_buffer_process(CommandBuffer* command_buffer)
{
  //swap. crit sec
  
  RWLock_w_lock(&command_buffer->rw_lock);
  command_buffer->num_commands[command_buffer->current_buffer] = 0;
  command_buffer->current_buffer = OTHER_BUFFER(command_buffer->current_buffer);
  RWLock_w_unlock(&command_buffer->rw_lock);

  printf("command_buffer_process: back buffer %i, current buffer %i\n", OTHER_BUFFER(command_buffer->current_buffer), command_buffer->current_buffer);
  
  int current_buffer = command_buffer->current_buffer;
  // Get current array
  Command* command_array = command_buffer->commands[current_buffer];

  if (command_buffer->num_commands[current_buffer] > command_buffer->max_commands)
    command_buffer->num_commands[current_buffer] = command_buffer->max_commands;
  size_t num_commands = command_buffer->num_commands[current_buffer];
  
  Command* command;
  for (size_t i = 0; i < num_commands; i++)
  {
    command = command_buffer->commands[current_buffer] + i;
    command->dispatch_function(command->params, command->data, command->size);
  }

  return NO_ERROR;
}

void destroy_command_buffer(CommandBuffer* command_buffer)
{
  RWLock_destroy(&command_buffer->rw_lock);
}
