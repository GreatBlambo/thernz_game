#include <handle.h>

void create_handle_array(HandleArray* handle_array, const size_t max_handles, Buffer* buffer)
{
  ASSERT(max_handles > HANDLE_INDEX_MASK, "The absolute maximum number of handles is %u\n", HANDLE_INDEX_MASK);
  
  handle_array->generations = push_array(uint16_t, buffer, max_handles);
  create_queue(&handle_array->free_indices, max_handles, buffer);

  ASSERT(handle_array->generations, "Not enough space");
  
  handle_array->num_handles = 0;
  handle_array->max_handles = max_handles;
  handle_array->min_free_indicies = max_handles / 10;

  zero_array(handle_array->generations, max_handles);
}

static uint32_t pop_free_index(HandleArray* handle_array)
{
  uint32_t result = queue_back(&handle_array->free_indices);
  queue_pop(&handle_array->free_indices);
  return result;
}

static void push_free_index(HandleArray* handle_array, uint32_t index)
{
  handle_array->generations[index] = (handle_array->generations[index] + 1) & HANDLE_GENERATION_MASK;
  queue_push(&handle_array->free_indices, index);
}

bool new_handle(HandleArray* handle_array, Handle* handle)
{
  uint32_t index;
  if (queue_size(&handle_array->free_indices) > handle_array->min_free_indicies)
  {
    index = pop_free_index(handle_array);
  }
  else if (handle_array->num_handles < handle_array->max_handles)
  {
    index = handle_array->num_handles;
    printf("num_handles = %u, num free indices = %u\n", index, queue_size(&handle_array->free_indices));
    handle_array->num_handles++;
  }
  else
  {
    return false;
  }
  
  *handle = generate_handle(index, handle_array->generations[index]);
  return true;
}

void deactivate_handle(HandleArray* handle_array, Handle handle)
{
  uint32_t index = handle_get_index(handle);
  if (handle_array->generations[index] == handle_get_generation(handle))
  {
    push_free_index(handle_array, index);
  }
}

bool handle_is_active(HandleArray* handle_array, Handle handle)
{
  return handle_array->generations[handle_get_index(handle)] != handle_get_generation(handle);
}
