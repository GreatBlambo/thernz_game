#pragma once
#include "render_functions.h"
#include "graphics.h"

struct CommandBuffer
{
  Buffer* frame_memory;
};

void create_command_buffer(Buffer* perm_memory, Buffer* frame_memory)
{
}
