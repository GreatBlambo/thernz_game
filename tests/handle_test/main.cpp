#include <game.h>
#include <handle.h>

int main(int, char**)
{
  Buffer buffer;
  create_buffer(&buffer, malloc(GIGABYTE(1)), GIGABYTE(1));

  HandleArray handle_array;
  create_handle_array(&handle_array, 10, &buffer);

  for (int i = 0; i < handle_array.max_handles; i++)
  {
    Handle handle;     
    ASSERT(new_handle(&handle_array, &handle), "Exceeded handle limit\n");
    printf("index: %u, generation: %u\n", handle_get_index(handle), handle_get_generation(handle));
  }
  
  return 0;
}
