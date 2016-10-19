#pragma once
#include <stdint.h>

namespace tz
{
enum class InputEventType : unsigned
{
  INVALID,
  key,
  mouse_button,
  mouse_wheel,
  mouse_motion,
  app_close
};

enum class PressEvent : unsigned
{
  press,
  release
};

enum class KeyMod : unsigned
{
  none = 0,
  l_alt = 1,
  l_super = 1 << 1,
  l_shift = 1 << 2,
  l_ctrl = 1 << 3,
  r_alt = 1 << 4,
  r_super = 1 << 5,
  r_shift = 1 << 6,
  r_ctrl = 1 << 7
};

enum class MousePress : unsigned
{
  left,
  middle,
  right
};

enum class KeyPress : unsigned
{
  f1,
  f2,
  f3,
  f4,
  f5,
  f6,
  f7,
  f8,
  f9,
  f10,
  f11,
  f12,

  print_screen,
  insert,
  del,
  home,
  end,
  pgup,
  pgdn,
  tilde,
  escape,
  space,

  num_0,
  num_1,
  num_2,
  num_3,
  num_4,
  num_5,
  num_6,
  num_7,
  num_8,
  num_9,

  numpad_0,
  numpad_1,
  numpad_2,
  numpad_3,
  numpad_4,
  numpad_5,
  numpad_6,
  numpad_7,
  numpad_8,
  numpad_9,
  numpad_decimal,
  numpad_divide,
  numpad_multiply,
  numpad_subtract,
  numpad_add,
  numpad_enter,
  numpad_equal,

  minus,
  equals,
  backspace,
  enter,
  
  num_lock,
  scroll_lock,
  caps_lock,

  pause,
  
  forward_slash,
  back_slash,

  tab,
  left_shift,
  right_shift,
  left_ctrl,
  right_ctrl,
  left_alt,
  right_alt,
  left_super,
  right_super,

  dir_up,
  dir_down,
  dir_left,
  dir_right,

  left_bracket,
  right_bracket,
  semicolon,
  apostrophe,
  comma,
  period,

  a,
  b,
  c,
  d,
  e,
  f,
  g,
  h,
  i,
  j,
  k,
  l,
  m,
  n,
  o,
  p,
  q,
  r,
  s,
  t,
  u,
  v,
  w,
  x,
  y,
  z
};

struct KeyEvent
{
  KeyPress key;
  PressEvent press_event;
  KeyMod mod;
};

struct MousePosition
{
  int32_t x;
  int32_t y;
};

struct MouseMotion
{
  int32_t xrel;
  int32_t yrel;
};

struct MouseButtonEvent
{
  MousePress button;
  PressEvent press_event;
  MousePosition position;
  uint8_t clicks;
};

struct MouseWheelEvent
{
  int32_t x_scrolled;
  int32_t y_scrolled;
};

struct MouseMotionEvent
{
  MousePosition position;
  MouseMotion motion;
};

union InputEventData
{
  KeyEvent key_event;
  MouseButtonEvent mouse_button_data;
  MouseWheelEvent mouse_wheel_data;
  MouseMotionEvent mouse_motion_data;
};

struct InputEvent
{
  InputEventType type;
  InputEventData data;
};

bool poll_events(InputEvent& event);

};
