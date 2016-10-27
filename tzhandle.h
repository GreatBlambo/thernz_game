#pragma once

#include <thirdparty/bitsquid-foundation-git/array.h>
#include <thirdparty/bitsquid-foundation-git/queue.h>

#include "tzbitfield.h"

namespace tz
{
#define TZ_HANDLE(name, type, N)				\
  TZ_BITFIELD_START(name, type);				\
  TZ_BITFIELD_ENTRY(index, N, 0);				\
  TZ_BITFIELD_ENTRY(generation, (sizeof(type) * 8) - N, N);	\
  TZ_BITFIELD_END(name);
  
  template <typename HandleType>
  class HandleSet
  {
  public:
    HandleSet(size_t reserve_handles, size_t min_free_indices, foundation::Allocator& allocator)
      : m_alloc(allocator)
      , m_generations(allocator)
      , m_free_indices(allocator)
      , m_min_free_indices(min_free_indices)
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
      if (foundation::queue::size(m_free_indices) >= m_min_free_indices)
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

      if (index >= HandleType::max_index())
      {
	error("No more indices available\n");
	handle.set_generation(-1);
      }
      else
      {
	handle.set_generation(m_generations[index]);
      }

      return handle;
    }

    void destroy_handle(HandleType handle)
    {
      size_t index = handle.index();
      foundation::queue::push_back(m_free_indices, index);
      m_generations[index]++;
    }
    
  private:
    foundation::Array<uint32_t> m_generations;
    foundation::Queue<size_t> m_free_indices;

    size_t m_min_free_indices;
    
    foundation::Allocator& m_alloc;
  };
};
