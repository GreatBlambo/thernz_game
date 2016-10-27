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
  init(TZ_GIGABYTE(1), TZ_GIGABYTE(1), RENDERING);
  renderer::set_window_name("test");
  renderer::set_window_size(800, 600);

  foundation::memory_globals::init();

  renderer::CommandBuffer<uint64_t> command_buffer(foundation::memory_globals::default_allocator());

  ShaderID vert, frag;
  ShaderProgramID program;
  VertexArrayID vao;
  VertexBufferID vbo;
  IndexArrayID ibo;

  VertexAttribArray vert_spec(foundation::memory_globals::default_allocator());
  foundation::array::reserve(vert_spec, 2);
  push_attrib(vert_spec, "position", 0, 4, FLOAT);
  push_attrib(vert_spec, "color", 1, 4, FLOAT);
  
  vert = renderer::load_shader_source("assets/shaders/test.vert", VERTEX_SHADER);
  frag = renderer::load_shader_source("assets/shaders/test.frag", FRAGMENT_SHADER);
  
  ShaderID shaders[] = { vert, frag };
  program = renderer::link_shader_program(shaders, 2, vert_spec);

  // Replace this with vao gen, buffer update/generation
  // buffer handle + data -> renderer -> vbo
  // vertex spec -> renderer -> vao handle

  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ibo);
  glGenVertexArrays(1, &vao);
  
  glBindVertexArray(vao);
  
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), (void*) Vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), (void*) Indices, GL_STATIC_DRAW);
  
  for (size_t i = 0; i < foundation::array::size(vert_spec); i++)
  {
    VertexAttribute& attrib = vert_spec[i];
    glEnableVertexAttribArray(attrib.location);
    glVertexAttribPointer(attrib.location, attrib.size,
                          attrib.type, false, vert_spec.vert_size, (const void*) attrib.offset);
  }
  
  glBindVertexArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  
  //
  
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

    renderer::Command* command = command_buffer.push_command<renderer::DrawIndexed>(0, 0);
    renderer::Command::set_function(command, renderer::DrawIndexed::dispatch);
    renderer::DrawIndexed* dc = renderer::Command::get_data<renderer::DrawIndexed>(command);
    dc->vao = vao;
    dc->draw_type = TRIANGLES;
    dc->start_index = 0;
    dc->num_indices = TZ_ARRAY_SIZE(Indices);
    dc->indices_type = UNSIGNED_BYTE;
    dc->material.shader = program;
    dc->material.num_textures = 0;
    dc->instances = 0;

    command_buffer.sort();
    command_buffer.submit();
    command_buffer.reset();

    frame();
  }
  
  deinit();
}
