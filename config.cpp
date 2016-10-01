#include <config.h>

static SDL_Rect window_rect = {
  .x = SDL_WINDOWPOS_CENTERED,
  .y = SDL_WINDOWPOS_CENTERED,
  .w = SCREEN_WIDTH,
  .h = SCREEN_HEIGHT
};

WindowParams g_window_params = { GAME_NAME, window_rect, FULLSCREEN };
