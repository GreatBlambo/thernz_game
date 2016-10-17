#pragma once

#include <memory.h>

namespace tz
{  
  template <typename T>
    struct Queue
    {
      size_t offset;
      size_t size;
      size_t capacity;
      T* data;
    };

  template <typename T>
    void create_queue(Queue<T>* q, T* array, size_t capacity)
    {
      q->data = array;
      q->capacity = capacity;
      q->size = 0;
    }

  template <typename T>
    void create_queue(Queue<T>* q, size_t capacity, Buffer* buffer)
    {
      T* array = push_array(T, buffer, capacity);
      TZ_ASSERT(array, "Buffer over capacity");
      zero_array(array, capacity);

      q->offset = 0;

      create_queue(q, array, capacity);
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
    bool queue_pop(Queue<T>* q)
    {
      if (q->size <= 0)
        return false;
      q->size--;
      q->offset = (q->offset + 1) % q->capacity;
      return true;
    }

  template <typename T>
    T queue_back(Queue<T>* q)
    {
      return q->data[q->offset % q->capacity];
    }

  template <typename T>
    void queue_reset(Queue<T>* q)
    {
      q->offset = 0;
      q->size = 0;
    }
}
