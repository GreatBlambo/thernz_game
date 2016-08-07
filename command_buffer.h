#pragma once
#include <atomic>
#include <thread>

#include "memory.h"
#include "rw_lock.h"
#include "error_codes.h"

////////////////////////////////////////////////////////////////////////////////
// Command buffer
////////////////////////////////////////////////////////////////////////////////

#define OTHER_BUFFER(index) !index
#define NUM_BUFFERS 2
typedef void (*DispatchFunction) (void* params, void* data, size_t size);

struct Command
{
  DispatchFunction dispatch_function;
  void* params;
  void* data;
  size_t size;
};

struct CommandBuffer
{
  FrameDataBuffer* frame_memory;
  Command* commands[NUM_BUFFERS];
  std::atomic_size_t num_commands[NUM_BUFFERS];
  bool current_buffer;
  
  size_t max_commands;

  RWLock rw_lock;
};

/**
 * Submit command with existing data.
 */

template <typename Params>
GameError command_buffer_submit(CommandBuffer* command_buffer,
				DispatchFunction dispatch_function,
				Params* params, void* data = NULL, size_t data_size = 0)
{
  RWLock_r_lock(&command_buffer->rw_lock);
  
  bool back_buffer = OTHER_BUFFER(command_buffer->current_buffer);
  // Get current array
  Command* command_array = command_buffer->commands[back_buffer];

  // Get offset
  // Gets CURRENT value of num_commands and atomically adds to it.
  size_t current_index = command_buffer->num_commands[back_buffer]++;
  
  // If that value was previously greater than the maximum allowed commands, reset to max and
  // return
  if (current_index >= command_buffer->max_commands)
  {
    command_buffer->num_commands[back_buffer] = command_buffer->max_commands;
    return ERROR_ARRAY_SIZE;
  }

  void* command_params = NULL;
  if (params)
  {
    command_params = push_struct<Params>(command_buffer->frame_memory);
    if (!command_params)
    {
      command_buffer->num_commands[back_buffer]--;
      return ERROR_BUFFER_SIZE;
    }
    *((Params*) command_params) = *params;
  }

  Command* command = command_array + current_index;
  printf("submit: %p, %i, back buff %i\n", command, current_index, back_buffer);
  command->dispatch_function = dispatch_function;
  printf("post submit: %p, %i, back buff %i\n", command, current_index, back_buffer);
  command->params = (void*) command_params;
  command->data = data;
  command->size = data_size;
  
  RWLock_r_unlock(&command_buffer->rw_lock);
  
  return NO_ERROR;
}

/**
 * Allocate and copy data to frame memory
 */

template <typename Params>
GameError command_buffer_alloc_and_submit(CommandBuffer* command_buffer,
					  DispatchFunction dispatch_function,
					  Params* params, void* data, size_t data_size)
{
  void* new_data = NULL;
  if (data)
  {
    new_data = push_size(command_buffer->frame_memory, data_size);
    if (!new_data)
      return ERROR_BUFFER_SIZE;
    memcpy(new_data, data, data_size);
  }
  command_buffer_submit(command_buffer, dispatch_function, params, new_data, data_size);
  return NO_ERROR;
}

GameError command_buffer_process(CommandBuffer* command_buffer);
GameError create_command_buffer(CommandBuffer* result, size_t max_commands,
				Buffer* perm_memory, FrameDataBuffer* frame_memory);
void destroy_command_buffer(CommandBuffer* command_buffer);
