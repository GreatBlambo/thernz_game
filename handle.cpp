#include <handle.h>

// TODO: REPLACE STACK SHIT WITH QUEUE!!

void create_handle_array(HandleArray* handle_array, const size_t max_handles, Buffer* buffer)
{
  ASSERT(max_handles > HANDLE_INDEX_MASK, "The absolute maximum number of handles is %u\n", HANDLE_INDEX_MASK);
  
  handle_array->generations = push_array(uint16_t, buffer, max_handles);
  handle_array->free_indices = push_array(size_t, buffer, max_handles);
  ASSERT(handle_array->generations, "Not enough space");
  ASSERT(handle_array->free_indices, "Not enough space");
  handle_array->num_handles = 0;
  handle_array->max_handles = max_handles;
  handle_array->num_gaps = 0;
  handle_array->min_free_indicies = max_handles / 10;

  zero_array(handle_array->generations, max_handles);
  zero_array(handle_array->free_indices, max_handles);
}

static uint32_t pop_free_index(HandleArray* handle_array)
{
  uint32_t result = handle_array->next_free_index;
  handle_array->next_free_index = handle_array->free_indices[result];
  handle_array->num_gaps--;
  return result;
}

static void push_free_index(HandleArray* handle_array, uint32_t index)
{
  handle_array->generations[index] = (handle_array->generations[index] + 1) & HANDLE_GENERATION_MASK;
  handle_array->free_indices[index] = handle_array->next_free_index;
  handle_array->next_free_index = index;
  handle_array->num_gaps++;
}

bool new_handle(HandleArray* handle_array, Handle* handle)
{
  uint32_t index;
  if (handle_array->num_gaps > handle_array->min_free_indicies)
  {
    index = pop_free_index(handle_array);
  }
  else if (handle_array->num_handles < handle_array->max_handles)
  {
    index = handle_array->num_handles;
    printf("num_handles = %u, num_gaps = %u\n", index, handle_array->num_gaps);
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
