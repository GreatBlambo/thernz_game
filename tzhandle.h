#pragma once

#include <thirdparty/bitsquid-foundation-git/array.h>
#include <thirdparty/bitsquid-foundation-git/queue.h>

#include "tzbitfield.h"
#include "tzerror_codes.h"

namespace tz
{
#define TZ_HANDLE(name, type, N)				\
  TZ_BITFIELD_START(name, type);				\
  TZ_BITFIELD_ENTRY(index, N, 0);				\
  TZ_BITFIELD_ENTRY(generation, (sizeof(type) * 8) - N, N);	\
  TZ_BITFIELD_END(name);	
  
  template <typename HandleType, size_t MIN_FREE_INDICES = 0>
    class HandleSet
    {
    public:
    HandleSet(size_t reserve_handles, foundation::Allocator& allocator)
    : m_alloc(allocator)
    , m_generations(allocator)
    , m_free_indices(allocator)
    {
      reserve(reserve_handles);
    }
    
    HandleSet(foundation::Allocator& allocator)
    : m_alloc(allocator)
    , m_generations(allocator)
    , m_free_indices(allocator)
    {}

    void reserve(size_t reserve_handles)
    {
      foundation::array::reserve(m_generations, reserve_handles);
      foundation::queue::reserve(m_free_indices, reserve_handles);
    }
    
    bool handle_is_valid(HandleType handle)
    {
      return handle.generation() == m_generations[handle.index()];
    }

    HandleType create_handle()
    {
      HandleType handle;
      size_t index;
      if (foundation::queue::size(m_free_indices) > MIN_FREE_INDICES)
        {
          index = *foundation::queue::begin_front(m_free_indices);
          foundation::queue::pop_front(m_free_indices);
        }
      else
        {
          index = foundation::array::size(m_generations);
          foundation::array::push_back(m_generations, (uint32_t) 0);
        }

      handle.set_index(index);

      TZ_ASSERT(index < HandleType::max_index, "No more indices available\n");
      
      handle.set_generation(m_generations[index]);

      return handle;
    }

    void destroy_handle(HandleType handle)
    {
      size_t index = handle.index();
      foundation::queue::push_back(m_free_indices, index);
      m_generations[index]++;
      TZ_ASSERT(m_generations[index] <= HandleType::max_generation, "No more generations available\n");
    }
    
    private:
    foundation::Array<uint32_t> m_generations;
    foundation::Queue<size_t> m_free_indices;
    
    foundation::Allocator& m_alloc;
    };

  template <typename HandleType, typename Value, size_t MIN_FREE_INDICES = 0>
  class HandleMap
  {
  public:

  HandleMap(size_t reserve, foundation::Allocator& allocator)
  : m_handle_set(reserve, allocator)
  , m_values(allocator)
  {
    reserve(reserve);
  }
  
  HandleMap(foundation::Allocator& allocator)
  : m_handle_set(allocator)
  , m_values(allocator)
  {}

  void reserve(size_t reserve)
  {
    foundation::array::reserve(m_values, reserve);
  }

  HandleType add(Value value)
  {
    HandleType handle = m_handle_set.create_handle();
    size_t index = handle.index();
    
    if (index == foundation::array::size(m_values) + 1)
      foundation::array::push_back(m_values, value);
    else if (index < foundation::array::size(m_values))
      m_values[index] = value;
    else
      TZ_ASSERT(false, "Handle index is invalid");

    return handle;
  }

  void remove(HandleType handle)
  {
    m_handle_set.destroy_handle(handle);
  }

  bool has(HandleType handle)
  {
    return m_handle_set.handle_is_valid(handle);
  }
    
  Value& get(HandleType handle)
  {    
    size_t index = handle.index();
    TZ_ASSERT(index < foundation::array_size(m_values), "Handle index is invalid");

    return m_values[index];
  }
  
  private:
  HandleSet<HandleType, MIN_FREE_INDICES> m_handle_set;
  foundation::Array<Value> m_values;
  };
};
