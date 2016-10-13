#include <tzengine.h>

int main(int, char**)
{
  tz::init(GIGABYTE(1), GIGABYTE(1), tz::GRAPHICS);
  tz::set_window_name("test");
  tz::set_window_size(800, 600);

  bool quit = false;

  tz::InputEvent input_event;
  while(!quit)
  {
    tz::poll_events(input_event);
    switch(input_event.type)
    {
    case tz::InputEventType::app_close:
      quit = true;
      break;
    case tz::InputEventType::key:
      switch (input_event.data.key_event.key)
      {
      case tz::KeyPress::escape:
        quit = true;
        break;
      }
      break;
    }
    tz::flush_frame();
  }
  
  tz::deinit();
}
