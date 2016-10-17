#pragma once

namespace tz
{
  namespace graphics
  {
    
  GameError init_graphics();
  void deinit_graphics();
  
  void set_window_name(const char* name);
  void set_window_size(int width, int height);
  void set_window_fullscreen();
  void set_window_fullscreen_windowed();
  void set_window_windowed();

  void swap_backbuffer();
  
  }
}
