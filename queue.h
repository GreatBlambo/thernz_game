#pragma once

#include <memory.h>

template <typename T>
struct Queue
{
  size_t offset;
  size_t size;
  size_t capacity;
  T* data;
};

template <typename T>
void create_queue(Queue<T>* q, size_t capacity, Buffer* buffer)
{
  q->data = push_array(T, buffer, capacity);
  ASSERT(q->data, "Buffer over capacity");
  zero_array(q->data, capacity);

  q->offset = 0;
  q->size = 0;
  q->capacity = capacity;
}

template <typename T>
size_t queue_size(Queue<T>* q)
{
  return q->size;
}

template <typename T>
bool queue_push(Queue<T>* q, T val)
{
  if (q->size >= q->capacity)
    return false;
  q->data[(q->size + q->offset) % q->capacity] = val;
  q->size = q->size + 1;  
  return true;
}

template <typename T>
void queue_pop(Queue<T>* q)
{
  if (q->size <= 0)
    return;
  q->size--;
  q->offset = (q->offset + 1) % q->capacity;
}

template <typename T>
T queue_back(Queue<T>* q)
{
  return q->data[q->offset % q->capacity];
}
