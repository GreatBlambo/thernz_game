#include <tzengine.h>

int main(int, char**)
{
  tz::init(TZ_GIGABYTE(1), TZ_GIGABYTE(1), tz::GRAPHICS | tz::RENDERING);
  tz::graphics::set_window_name("test");
  tz::graphics::set_window_size(800, 600);

  bool quit = false;

  tz::InputEvent input_event;
  while(!quit)
  {
    while (tz::poll_events(input_event))
    {
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
    }
    tz::frame();
  }
  
  tz::deinit();
}
