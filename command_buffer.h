#include <atomic>
#include <thread>

#include "memory.h"
#include "error_codes.h"
#include "sort.h"

////////////////////////////////////////////////////////////////////////////////
// Command buffer
////////////////////////////////////////////////////////////////////////////////
/**
 * A buffer for adding commands in parallel and synchronously executing them.
 */

template <typename Key>
using DispatchFunction = void (*) (Key key, void* params, void* data, size_t size);

template <typename Key>
struct GenericCommand
{
  DispatchFunction<Key> dispatch_function;
  void* params;
  void* data;
  size_t size;
};

template <typename Key>
struct CommandArray
{
  SortItem<Key>* sort_keys;
  GenericCommand<Key>* commands;
  std::atomic_size_t num_commands;
};

template <typename Key>
struct GenericCommandBuffer
{
  FrameDataBuffer* frame_memory;
  SortItem<Key>* result_keys;

  CommandArray<Key> command_arrays;
  
  size_t max_commands;
}; 

template <typename Key>
GameError create_command_buffer(GenericCommandBuffer<Key>* result, size_t max_commands, Buffer* perm_memory, FrameDataBuffer* frame_memory)
{
  // Check if frame_memory can support requested commands and if we can allocate enough memory for keys
  size_t amount_required = 0;
  amount_required += sizeof(SortItem<Key>); // Result keys
  amount_required += (sizeof(SortItem<Key>) + sizeof(GenericCommand<Key>)); //CommandArrays
  amount_required *= max_commands; // times max commands
  
  if (frame_memory->size < max_commands
      || buffer_get_remaining(perm_memory) < amount_required)
    return ERROR_BUFFER_SIZE;

  result->frame_memory = frame_memory;
  result->result_keys = push_array< SortItem<Key> >(perm_memory, max_commands);

  // Allocate the buffers
  result->command_arrays->sort_keys = push_array< SortItem<Key> >(perm_memory, max_commands);
  result->command_arrays->commands = push_array< GenericCommand<Key> >(perm_memory, max_commands);
  result->command_arrays->num_commands = 0;
  
  result->max_commands = max_commands;
  result->current_array = 0;
  result->num_writing = 0;
  result->is_swapping = 0;

  return NO_ERROR;
}

template <typename Key>
void* command_buffer_push_mem(GenericCommandBuffer<Key>* command_buffer, size_t size)
{
  return push_size(command_buffer->frame_memory, size);
}


/**
 * DO NOT SUBMIT COMMANDS WHILE PROCESSING
 */
template <typename Key>
GameError command_buffer_submit_command(GenericCommandBuffer<Key>* command_buffer,
                                        DispatchFunction<Key> dispatch_function,
                                        Key key, void* params, void* data, size_t size)
{  
  // Get current array
  CommandArray<Key>& command_array = command_buffer->command_arrays;

  // Get offset
  // Gets CURRENT value of num_commands and atomically adds to it.
  size_t current_index = command_array.num_commands.fetch_add();
  // If that value was previously greater than the maximum allowed commands, reset to max and
  // return
  if (current_index >= command_buffer->max_commands)
  {
    command_array.num_commands = command_buffer->max_commands;
    return ERROR_ARRAY_SIZE;
  }
  
  GenericCommand<Key>* command = command_array.commands + current_index;
  command->dispatch_function = dispatch_function;
  command->params = params;
  command->data = data;
  command->size = size;

  SortItem<Key>* sort_item = command_array.sort_keys + current_index;
  sort_item->index = current_index;
  sort_item->sort_key = key;
  
  return NO_ERROR;
}

/**
 * DO NOT SUBMIT COMMANDS WHILE PROCESSING
 */
template <typename Key>
GameError command_buffer_process(GenericCommandBuffer<Key>* command_buffer)
{  
  // Get current array
  CommandArray<Key>& command_array = command_buffer->command_arrays;

  size_t num_commands = command_array.num_commands;
  if (num_commands > command_array.max_commands)
    num_commands = command_array.max_commands;

  radix_sort(command_array.sort_keys, command_array.result_keys, num_commands);

  SortItem<Key> result_key;
  GenericCommand<Key>* command;
  for (size_t i = 0; i < num_commands; i++)
  {
    result_key = command_array.result_keys[i];
    command = command_array.commands + result_key.index;
    command->dispatch_function(result_key->key, command->params, command->data, command->size);
  }
  
  return NO_ERROR;
}
