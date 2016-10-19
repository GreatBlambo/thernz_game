#pragma once

#include <stdint.h>
#include <algorithm>

#include <glm/glm.hpp>
#include <thirdparty/bitsquid-foundation-git/array.h>

#include "tzengine.h"
#include "tzconfig.h"
#include "tzrender_types.h"
#include "tzsort.h"

namespace tz
{
/**
 * Sorts and submits draw calls to the GPU. That's all this module does for now. We can sort out
 * solutions for thread safety and double buffering later.
 * 
 */
namespace renderer
{
  ////////////////////////////////////////////////////////////////////////////////
  // CommandBuffer
  ////////////////////////////////////////////////////////////////////////////////

  struct Command
  {
    typedef void (*DispatchFunction) (void* data);
    DispatchFunction dispatch;
    void* data;
    void* aux_data;
    size_t aux_data_size;
    Command* next_command;

    static Command* get_command(void* data)
    {
      return ((Command*) data) - 1;
    }
    
    static void* get_aux_data(void* data)
    {
      return get_command(data)->aux_data;
    }
    
    static size_t get_aux_size(void* data)
    {
      return get_command(data)->aux_data_size;
    }

  };
  
  template <typename T>
  class CommandBuffer
  {
  public:
        
    typedef T Key;
    CommandBuffer(foundation::Allocator& allocator, size_t data_size, size_t reserve_calls)
      : m_data_allocator(allocator.allocate(data_size), data_size)
      , m_base_allocator(allocator)
      , m_reserved_calls(reserve_calls)
      , m_num_calls(0)
    {
      reserve_draw_calls(reserve_calls);
    }

    ~CommandBuffer()
    {
      m_base_allocator.deallocate((void*) m_keys);
      m_base_allocator.deallocate((void*) m_commands);
    }

    void reset()
    {
      m_num_calls = 0;
    }
    
    void reserve_draw_calls(size_t n)
    {
      if (n <= m_reserved_calls)
        return;
      m_keys = (sort::SortKey<Key>*) m_base_allocator.allocate(n * sizeof(Key));
      m_commands = (Command**) m_base_allocator.allocate(n * sizeof(Command*));
        
      m_reserved_calls = n;
      reset();
    }
    
    template <typename DataType>
    DataType* push_command(Key key, size_t aux_size)
    {
      if (m_num_calls > m_reserved_calls) return NULL;
      Command* new_command = allocate_command(aux_size);
      
      // Add Command
      m_commands[m_num_calls] = new_command;
      
      // Add Key
      sort::SortKey<Key>* new_key = m_keys + m_num_calls;
      new_key->key = key;
      new_key->data = (void*) new_command;
      
      m_num_calls.fetch_add(1, std::memory_order_release);
      return (DataType*) new_command->data;
    }

    template <typename DataType1, typename DataType2>
    DataType2* chain_command(size_t aux_size, DataType1* parent)
    {
      Command* previous_command = Command::get_command((void*) parent);
      DataType2* new_entry = allocate_command<DataType2>(aux_size);

      previous_command->next_command = Command::get_command((void*) new_entry);

      return new_entry;
    }

    void sort()
    {
      std::sort(m_keys, m_keys + m_num_calls);
    }

    void submit()
    {
      for (size_t i = 0; i < m_num_calls; i++)
      {
        Command* command = (Command*) m_keys[m_num_calls].data;
        do
        {
          command->dispatch(command->data);
          command = command->next_command;
        } while (command);
      }
    }
    
  private:      
    template <typename DataType>
    Command* allocate_command(size_t aux_size)
    {
      Command* new_command = m_data_allocator.allocate(sizeof(Command) + sizeof(DataType) + aux_size);
      DataType* data = (DataType*) (new_command + 1);
      void* aux_data = (void*) (data + 1);
      
      new_command->data = data;
      new_command->dispatch = DataType::function;
      new_command->aux_data = aux_data;
      new_command->aux_data_size = aux_size;
      new_command->next_command = NULL;

      return new_command;
    }

    sort::SortKey<Key>* m_keys;
    Command** m_commands;
    std::atomic_size_t m_num_calls;
    size_t m_reserved_calls;
    
    AtomicLinearAllocator m_data_allocator;
    foundation::Allocator& m_base_allocator;
  };

  ////////////////////////////////////////////////////////////////////////////////
  // Module interface
  ////////////////////////////////////////////////////////////////////////////////
  
  void init_rendering();
  void deinit_rendering();
  void push_frame();
  
  /**
   * Note: It's completely unecessary to sort by vao. Sorting by material groups draw calls with the
   * same vao together naturally. Additionally, by sharing buffers you can increase the chances
   * of not needing a vao switch.
   */
  typedef uint64_t Key;
  
  typedef uint32_t MaterialID;
  MaterialID create_material_id(ShaderProgramID program, TextureID* textures, size_t num_textures);

};
  
};
