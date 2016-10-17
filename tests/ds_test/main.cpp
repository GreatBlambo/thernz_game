#include <tzqueue.h>
#include <stdio.h>

void queue_test(size_t size)
{
  tz::Queue<int> q;
  tz::create_queue(&q, (int*) malloc(size * sizeof(int)), size);

  for (int i = 0; i < size; i++)
  {
    ASSERT(tz::queue_push(&q, i), "Queue push failed\n");
  }
  
  ASSERT(tz::queue_size(&q) == size, "Queue size: %u, should be %u", tz::queue_size(&q), size);

  for (int i = 0; i < size; i++)
  {
    ASSERT(tz::queue_back(&q) == i, "Queue back: %u, should be %i", tz::queue_back(&q), i);
    ASSERT(tz::queue_size(&q) == size - i, "Queue size: %u, should be %u", tz::queue_size(&q), size - i);
    ASSERT(tz::queue_pop(&q), "Queue pop failed");
  }

  ASSERT(tz::queue_size(&q) == 0, "Queue is not empty");
}

int main(int, char**)
{
  queue_test(100);
  return 0;
}
