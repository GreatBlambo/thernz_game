#include <tzengine.h>

using namespace tz;

struct Vertex
{
  glm::vec4 position;
  glm::vec4 color;
};

Vertex Vertices[] = // shape
{
	{ { 0.0f, 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f } },
	// Top
	{ { -0.2f, 0.8f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
	{ { 0.2f, 0.8f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
	{ { 0.0f, 0.8f, 0.0f, 1.0f }, { 0.0f, 1.0f, 1.0f, 1.0f } },
	{ { 0.0f, 1.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
	// Bottom
	{ { -0.2f, -0.8f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
	{ { 0.2f, -0.8f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
	{ { 0.0f, -0.8f, 0.0f, 1.0f }, { 0.0f, 1.0f, 1.0f, 1.0f } },
	{ { 0.0f, -1.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
	// Left
	{ { -0.8f, -0.2f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
	{ { -0.8f, 0.2f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
	{ { -0.8f, 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 1.0f, 1.0f } },
	{ { -1.0f, 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
	// Right
	{ { 0.8f, -0.2f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
	{ { 0.8f, 0.2f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
	{ { 0.8f, 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 1.0f, 1.0f } },
	{ { 1.0f, 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } }
};

GLubyte Indices[] = {
	// Top
	0, 1, 3,
	0, 3, 2,
	3, 1, 4,
	3, 4, 2,
	// Bottom
	0, 5, 7,
	0, 7, 6,
	7, 5, 8,
	7, 8, 6,
	// Left
	0, 9, 11,
	0, 11, 10,
	11, 9, 12,
	11, 12, 10,
	// Right
	0, 13, 15,
	0, 15, 14,
	15, 13, 16,
	15, 16, 14
};

int main(int, char**)
{
  init(TZ_GIGABYTE(1), TZ_GIGABYTE(1), GRAPHICS | RENDERING);
  graphics::set_window_name("test");
  graphics::set_window_size(800, 600);
  
  LinearAllocator alloc(allocate(TZ_CONFIG_COMMAND_BUFFER_MAX_SIZE),
                        TZ_CONFIG_COMMAND_BUFFER_MAX_SIZE); //Temp
  renderer::CommandBuffer<uint64_t>* command_buffer = TZ_push_new(renderer::CommandBuffer<uint64_t>,
                                                                  &g_game_state.main_memory,
                                                                  alloc,
                                                                  TZ_CONFIG_COMMAND_BUFFER_MAX_SIZE,
                                                                  TZ_CONFIG_MAX_DRAW_CALLS);

  
  ShaderID vert, frag;
  ShaderProgramID program;
  VertexArrayID vao;
  VertexBufferID vbo;
  IndexArrayID ibo;

  VertSpec vert_spec;
  
  const char* names[]= {
    "position",
    "color"
  };
  vert_spec.attrib_names = names;
  int locations[] = {
    0,
    1
  };
  
  vert_spec.attrib_locations = locations;
  vert_spec.num_attributes = 2;

  vert = load_shader_source("assets/shaders/test.vert", VERTEX_SHADER);
  frag = load_shader_source("assets/shaders/test.frag", FRAGMENT_SHADER);
  ShaderID shaders[] = { vert, frag };
  program = link_shader_program(shaders, 2, vert_spec);

  
  
  bool quit = false;
  InputEvent input_event;
  while(!quit)
  {
    while (poll_events(input_event))
    {
      switch(input_event.type)
      {
      case InputEventType::app_close:
        quit = true;
        break;
      case InputEventType::key:
        switch (input_event.data.key_event.key)
        {
        case KeyPress::escape:
          quit = true;
          break;
        }
        break;
      }
    }
    frame();
  }
  
  deinit();
}
