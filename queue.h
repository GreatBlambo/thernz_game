#pragma once

#include <memory.h>

template <typename T>
struct Queue
{
  size_t head;
  size_t tail;
  size_t capacity;
  T* data;
};

template <typename T>
void create_queue(Queue<T>* q, size_t capacity, Buffer* buffer)
{
  q->data = push_array(T, buffer, capacity);
  ASSERT(q->data, "Buffer over capacity");

  q->head = 0;
  q->tail = 0;
  q->capacity = capacity;
}

