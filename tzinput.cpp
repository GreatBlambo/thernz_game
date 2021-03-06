#include <SDL2/SDL.h>
#include "tzinput.h"

namespace tz
{ 
  static KeyPress parse_SDL_scan_code(SDL_Scancode sdl_scancode);
  static void parse_SDL_mouse_event(MouseButtonEvent& mouse_press, SDL_MouseButtonEvent& sdl_mouseButtonEvent);
  static void parse_SDL_key_event(KeyEvent& key_event, SDL_Keysym sdl_keysym);

  bool poll_events(InputEvent& event)
  {
    SDL_Event sdl_event;

    if (!SDL_PollEvent(&sdl_event))
      return false;

    switch(sdl_event.type)
    {
    case SDL_KEYDOWN:
      event.type = InputEventType::key;
      event.data.key_event.press_event = PressEvent::press;
      parse_SDL_key_event(event.data.key_event, sdl_event.key.keysym);
      break;
    case SDL_KEYUP:
      event.type = InputEventType::key;
      event.data.key_event.press_event = PressEvent::release;
      parse_SDL_key_event(event.data.key_event, sdl_event.key.keysym);
      break;
    case SDL_MOUSEBUTTONDOWN:
      event.type = InputEventType::mouse_button;
      event.data.mouse_button_data.press_event = PressEvent::press;
      parse_SDL_mouse_event(event.data.mouse_button_data, sdl_event.button);
      break;
    case SDL_MOUSEBUTTONUP:
      event.type = InputEventType::mouse_button;
      event.data.mouse_button_data.press_event = PressEvent::release;
      parse_SDL_mouse_event(event.data.mouse_button_data, sdl_event.button);
      break;
    case SDL_MOUSEMOTION:
      event.type = InputEventType::mouse_motion;
      event.data.mouse_motion_data.motion = { sdl_event.motion.xrel, sdl_event.motion.yrel };
      event.data.mouse_motion_data.position = { sdl_event.motion.x, sdl_event.motion.y };
      break;
    case SDL_MOUSEWHEEL:
      event.type = InputEventType::mouse_wheel;
      event.data.mouse_wheel_data = { sdl_event.wheel.x, sdl_event.wheel.y };
      break;
    case SDL_QUIT:
      event.type = InputEventType::app_close;
      break;
    default:
      event.type = InputEventType::INVALID;
      return false;
    }
  
    return true;
  }


  void parse_SDL_mouse_event(MouseButtonEvent& mouse_press, SDL_MouseButtonEvent& sdl_mouseButtonEvent)
  {
    mouse_press.position = { sdl_mouseButtonEvent.x, sdl_mouseButtonEvent.y };
    mouse_press.clicks = sdl_mouseButtonEvent.clicks;
    switch(sdl_mouseButtonEvent.button)
    {
    case SDL_BUTTON_LEFT:
      mouse_press.button = MousePress::left;
      break;
    case SDL_BUTTON_MIDDLE:
      mouse_press.button = MousePress::middle;
      break;
    case SDL_BUTTON_RIGHT:
      mouse_press.button = MousePress::right;
      break;
    }
  }

  void parse_SDL_key_event(KeyEvent& key_event, SDL_Keysym sdl_keysym)
  {
    key_event.key = parse_SDL_scan_code(sdl_keysym.scancode);
    switch(sdl_keysym.mod)
    {
    case KMOD_NONE:
      key_event.mod = KeyMod::none;
      break;
    case KMOD_LSHIFT:
      key_event.mod = KeyMod::l_shift;
      break;
    case KMOD_RSHIFT:
      key_event.mod = KeyMod::r_shift;
      break;
    case KMOD_LCTRL:
      key_event.mod = KeyMod::l_ctrl;
      break;
    case KMOD_RCTRL:
      key_event.mod = KeyMod::r_ctrl;
      break;
    case KMOD_LALT:
      key_event.mod = KeyMod::l_alt;
      break;
    case KMOD_RALT:
      key_event.mod = KeyMod::r_alt;
      break;
    case KMOD_LGUI:
      key_event.mod = KeyMod::l_super;
      break;
    case KMOD_RGUI:
      key_event.mod = KeyMod::r_super;
      break;
    }
  }

  KeyPress parse_SDL_scan_code(SDL_Scancode sdl_scancode)
  {  
    switch(sdl_scancode)
    {
    case SDL_SCANCODE_A:				return KeyPress::a;
    case SDL_SCANCODE_B:				return KeyPress::b;
    case SDL_SCANCODE_C:				return KeyPress::c;
    case SDL_SCANCODE_D:				return KeyPress::d;
    case SDL_SCANCODE_E:				return KeyPress::e;
    case SDL_SCANCODE_F:				return KeyPress::f;
    case SDL_SCANCODE_G:				return KeyPress::g;
    case SDL_SCANCODE_H:				return KeyPress::h;
    case SDL_SCANCODE_I:				return KeyPress::i;
    case SDL_SCANCODE_J:				return KeyPress::j;
    case SDL_SCANCODE_K:				return KeyPress::k;
    case SDL_SCANCODE_L:				return KeyPress::l;
    case SDL_SCANCODE_M:				return KeyPress::m;
    case SDL_SCANCODE_N:				return KeyPress::n;
    case SDL_SCANCODE_O:				return KeyPress::o;
    case SDL_SCANCODE_P:				return KeyPress::p;
    case SDL_SCANCODE_Q:				return KeyPress::q;
    case SDL_SCANCODE_R:				return KeyPress::r;
    case SDL_SCANCODE_S:				return KeyPress::s;
    case SDL_SCANCODE_T:				return KeyPress::t;
    case SDL_SCANCODE_U:				return KeyPress::u;
    case SDL_SCANCODE_V:				return KeyPress::v;
    case SDL_SCANCODE_W:				return KeyPress::w;
    case SDL_SCANCODE_X:				return KeyPress::x;
    case SDL_SCANCODE_Y:				return KeyPress::y;
    case SDL_SCANCODE_Z:				return KeyPress::z;
    case SDL_SCANCODE_1:				return KeyPress::num_1;
    case SDL_SCANCODE_2:				return KeyPress::num_2;
    case SDL_SCANCODE_3:				return KeyPress::num_3;
    case SDL_SCANCODE_4:				return KeyPress::num_4;
    case SDL_SCANCODE_5:				return KeyPress::num_5;
    case SDL_SCANCODE_6:				return KeyPress::num_6;
    case SDL_SCANCODE_7:				return KeyPress::num_7;
    case SDL_SCANCODE_8:				return KeyPress::num_8;
    case SDL_SCANCODE_9:				return KeyPress::num_9;
    case SDL_SCANCODE_0:				return KeyPress::num_0;
    case SDL_SCANCODE_RETURN:			return KeyPress::enter;
    case SDL_SCANCODE_ESCAPE:			return KeyPress::escape;
    case SDL_SCANCODE_BACKSPACE:		        return KeyPress::backspace;
    case SDL_SCANCODE_TAB:			return KeyPress::tab;
    case SDL_SCANCODE_SPACE:			return KeyPress::space;
    case SDL_SCANCODE_MINUS:			return KeyPress::minus;
    case SDL_SCANCODE_EQUALS:			return KeyPress::equals;
    case SDL_SCANCODE_LEFTBRACKET:		return KeyPress::left_bracket;
    case SDL_SCANCODE_RIGHTBRACKET:		return KeyPress::right_bracket;
    case SDL_SCANCODE_BACKSLASH:		        return KeyPress::back_slash;
    case SDL_SCANCODE_SEMICOLON:		        return KeyPress::semicolon;
    case SDL_SCANCODE_APOSTROPHE:		        return KeyPress::apostrophe;
    case SDL_SCANCODE_GRAVE:			return KeyPress::tilde;
    case SDL_SCANCODE_COMMA:			return KeyPress::comma;
    case SDL_SCANCODE_PERIOD:			return KeyPress::period;
    case SDL_SCANCODE_SLASH:			return KeyPress::forward_slash;
    case SDL_SCANCODE_CAPSLOCK:			return KeyPress::caps_lock;
    case SDL_SCANCODE_F1:				return KeyPress::f1;
    case SDL_SCANCODE_F2:				return KeyPress::f2;
    case SDL_SCANCODE_F3:				return KeyPress::f3;
    case SDL_SCANCODE_F4:				return KeyPress::f4;
    case SDL_SCANCODE_F5:				return KeyPress::f5;
    case SDL_SCANCODE_F6:				return KeyPress::f6;
    case SDL_SCANCODE_F7:				return KeyPress::f7;
    case SDL_SCANCODE_F8:				return KeyPress::f8;
    case SDL_SCANCODE_F9:				return KeyPress::f9;
    case SDL_SCANCODE_F10:			return KeyPress::f10;
    case SDL_SCANCODE_F11:			return KeyPress::f11;
    case SDL_SCANCODE_F12:			return KeyPress::f12;
    case SDL_SCANCODE_PRINTSCREEN:		return KeyPress::print_screen;
    case SDL_SCANCODE_SCROLLLOCK:		        return KeyPress::scroll_lock;
    case SDL_SCANCODE_PAUSE:			return KeyPress::pause;
    case SDL_SCANCODE_INSERT:			return KeyPress::insert;
    case SDL_SCANCODE_HOME:			return KeyPress::home;
    case SDL_SCANCODE_PAGEUP:			return KeyPress::pgup;
    case SDL_SCANCODE_DELETE:			return KeyPress::del;
    case SDL_SCANCODE_END:			return KeyPress::end;
    case SDL_SCANCODE_PAGEDOWN:			return KeyPress::pgdn;
    case SDL_SCANCODE_RIGHT:			return KeyPress::dir_right;
    case SDL_SCANCODE_LEFT:			return KeyPress::dir_left;
    case SDL_SCANCODE_DOWN:			return KeyPress::dir_down;
    case SDL_SCANCODE_UP:				return KeyPress::dir_up;
    case SDL_SCANCODE_NUMLOCKCLEAR:		return KeyPress::num_lock;
    case SDL_SCANCODE_KP_DIVIDE:		        return KeyPress::numpad_divide;
    case SDL_SCANCODE_KP_MULTIPLY:		return KeyPress::numpad_multiply;
    case SDL_SCANCODE_KP_MINUS:			return KeyPress::numpad_subtract;
    case SDL_SCANCODE_KP_PLUS:			return KeyPress::numpad_add;
    case SDL_SCANCODE_KP_ENTER:			return KeyPress::numpad_enter;
    case SDL_SCANCODE_KP_1:			return KeyPress::numpad_1;
    case SDL_SCANCODE_KP_2:			return KeyPress::numpad_2;
    case SDL_SCANCODE_KP_3:			return KeyPress::numpad_3;
    case SDL_SCANCODE_KP_4:			return KeyPress::numpad_4;
    case SDL_SCANCODE_KP_5:			return KeyPress::numpad_5;
    case SDL_SCANCODE_KP_6:			return KeyPress::numpad_6;
    case SDL_SCANCODE_KP_7:			return KeyPress::numpad_7;
    case SDL_SCANCODE_KP_8:			return KeyPress::numpad_8;
    case SDL_SCANCODE_KP_9:			return KeyPress::numpad_9;
    case SDL_SCANCODE_KP_0:			return KeyPress::numpad_0;
    case SDL_SCANCODE_KP_PERIOD:		        return KeyPress::numpad_decimal;
    case SDL_SCANCODE_KP_EQUALS:		        return KeyPress::numpad_equal;
    case SDL_SCANCODE_LCTRL:			return KeyPress::left_ctrl;
    case SDL_SCANCODE_LSHIFT:			return KeyPress::left_shift;
    case SDL_SCANCODE_LALT:			return KeyPress::left_alt;
    case SDL_SCANCODE_LGUI:			return KeyPress::left_super;
    case SDL_SCANCODE_RCTRL:			return KeyPress::right_ctrl;
    case SDL_SCANCODE_RSHIFT:			return KeyPress::right_shift;
    case SDL_SCANCODE_RALT:			return KeyPress::right_alt;
    case SDL_SCANCODE_RGUI:			return KeyPress::right_super;
    default:
      return static_cast<KeyPress>(-1);
    }
  }
};
