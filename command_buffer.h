#include <atomic>

#include "memory.h"
#include "error_codes.h"
#include "sort.h"

template<typename Key>
using DispatchFunction = void (*) (Key key, void* data, size_t size);

template<typename Key>
struct GenericCommand
{
  DispatchFunction<Key> dispatch_function;
  void* data;
  size_t size;
};

template<typename Key>
struct GenericCommandBuffer
{
  FrameDataBuffer* frame_memory;
  GenericCommand<Key>* commands;
  SortItem<Key>* sort_keys;

  std::atomic_size_t num_commands;
  size_t max_commands;
}; 

template<typename Key>
GameError create_command_buffer(GenericCommandBuffer<Key>* result, size_t max_commands, Buffer* perm_memory, FrameDataBuffer* frame_memory)
{
  // Check if frame_memory can support requested commands and if we can allocate enough memory for keys
  if (frame_memory->size < max_commands
      || buffer_get_remaining(perm_memory) < (sizeof(SortItem<Key>) + sizeof(GenericCommand<Key>)) * max_commands)
    return ERROR_BUFFER_SIZE;

  result->frame_memory = frame_memory;
  result->commands = push_array< GenericCommand<Key> >(perm_memory, max_commands);
  result->sort_keys = push_array< SortItem<Key> >(perm_memory, max_commands);
  result->num_commands = 0;
  result->max_commands = max_commands;

  return NO_ERROR;
}

template<typename Key>
void* command_buffer_push_mem(GenericCommandBuffer<Key>* command_buffer, size_t size)
{
  return push_size(command_buffer->frame_memory, size);
}

template<typename Key>
GameError command_buffer_submit_command(GenericCommandBuffer<Key>* command_buffer,
                                        DispatchFunction<Key> dispatch_function,
                                        Key key, void* data, size_t size)
{
  size_t current_index = command_buffer->num_commands;
  if (command_buffer->num_commands.fetch_add() >= command_buffer->max_commands)
  {
    command_buffer->num_commands = command_buffer->max_commands;
    return ERROR_ARRAY_SIZE;
  }

  GenericCommand<Key>* command = command_buffer->commands + current_index;
  command->dispatch_function = dispatch_function;
  command->data = data;
  command->data = size;

  SortItem<Key>* sort_item = command_buffer->sort_keys + current_index;
  sort_item->index = current_index;
  sort_item->sort_key = key;
  
  return NO_ERROR;
}

template<typename Key>
GameError command_buffer_render(GenericCommandBuffer<Key>* command_buffer)
{
  if (buffer_get_remaining(&command_buffer->frame_memory) < sizeof(SortItem<Key>) * command_buffer->num_commands)
    return ERROR_BUFFER_SIZE;

  size_t num_commands = command_buffer->num_commands;
  SortItem<Key>* result_keys = push_array(&command_buffer->frame_memory, num_commands);  

  radix_sort(command_buffer->sort_keys, result_keys, num_commands);
  
  for (size_t i = 0; i < num_commands; i++)
  {
    SortItem<Key> result_key = result_keys[i];
    GenericCommand<Key>* command = command_buffer->commands + result_key.index;
    command->dispatch_function(result_key->key, command->data, command->size);
  }

  return NO_ERROR;
}
