#include <tzhandle.h>

#define MAX_HANDLES 10
#define MIN_FREE 5

TZ_HANDLE(Handle, uint32_t, 21);

int main(int, char**)
{
  foundation::memory_globals::init();
  tz::HandleSet<Handle> handle_set(MAX_HANDLES, MIN_FREE, foundation::memory_globals::default_allocator());

  Handle handles[MAX_HANDLES];
  for (size_t i = 0; i < MAX_HANDLES; i++)
  {
    handles[i] = handle_set.create_handle();
    TZ_ASSERT(handles[i].index() == i, "Index should be %u, is actually %u", i, handles[i].index());
    printf("Handle created. Generation = %u, index = %u\n", handles[i].generation(), handles[i].index());
  }

  for (size_t i = 0; i < MIN_FREE; i++)
  {
    handle_set.destroy_handle(handles[i]);
    TZ_ASSERT(!handle_set.handle_is_valid(handles[i]), "Destroyed handle is marked as valid.\n");
  }

  for (size_t i = 0; i < MIN_FREE; i++)
  {
    Handle handle = handles[i] = handle_set.create_handle();
    TZ_ASSERT(handle.generation() == 2, "Generation failure\n");
  }

  Handle handle = handle_set.create_handle();
  TZ_ASSERT(handle.generation() == 0, "Generation should be %u\n", 0);
  TZ_ASSERT(handle.index() == MAX_HANDLES + 1, "Index should be %u\n", MAX_HANDLES + 1);
  
};
