#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define NO_ERROR 0
#define ERROR_SDL 1
#define ERROR_NULL_PARAM 2

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

struct GameWindow
{
  SDL_Window* window;
  SDL_Surface* screen_surface;
  SDL_Renderer* renderer;
};

int init_game(GameWindow* game_window);
void destroy_game(GameWindow* game_window);

SDL_Surface* load_as_surface(const char* pathname, const SDL_Surface* screen_surface);
SDL_Texture* load_as_texture(const char* pathname, SDL_Renderer* renderer);

int main(int, char**)
{
  GameWindow game_window;
  int err = init_game(&game_window);
  if (err)
    return 1;
  
  SDL_Texture* stork_image = load_as_texture("assets/bird.png", game_window.renderer);
  if (!stork_image)
    return 1;
  SDL_Rect stretch_rect;
  stretch_rect.x = 0;
  stretch_rect.y = 0;
  stretch_rect.w = SCREEN_WIDTH;
  stretch_rect.h = SCREEN_HEIGHT;
  
  bool quit = false;
  SDL_Event e;
  while (!quit)
  {
    while (SDL_PollEvent(&e) != 0)
    {
      if (e.type == SDL_QUIT)
        quit = true;
      if (e.type == SDL_KEYDOWN)
      {
        if (e.key.keysym.sym == SDLK_ESCAPE)
          quit = true;
      }
    }
    SDL_RenderClear(game_window.renderer);
    SDL_RenderCopy(game_window.renderer, stork_image, NULL, &stretch_rect);
    SDL_RenderPresent(game_window.renderer);
  }
  SDL_DestroyTexture(stork_image);
  
  destroy_game(&game_window);
  return 0;
}

SDL_Texture* load_as_texture(const char* pathname, SDL_Renderer* renderer)
{
  //The final texture
  SDL_Texture* new_texture = NULL;

  //Load image at specified path
  SDL_Surface* loaded_surface = IMG_Load(pathname);
  if(loaded_surface == NULL)
  {
    printf("Unable to load image %s! SDL_image Error: %s\n", pathname, IMG_GetError());
    return NULL;
  }
  //Create texture from surface pixels
  new_texture = SDL_CreateTextureFromSurface(renderer, loaded_surface);
  if(new_texture == NULL)
  {
    printf( "Unable to create texture from %s! SDL Error: %s\n", pathname, SDL_GetError());
  }
  
  //Get rid of old loaded surface
  SDL_FreeSurface( loaded_surface );

  return new_texture;
}

SDL_Surface* load_as_surface(const char* pathname, const SDL_Surface* screen_surface)
{
  if (!screen_surface)
    return NULL;
  //The final optimized image
  SDL_Surface* optimized_surface = NULL;

  //Load image at specified path
  SDL_Surface* loaded_surface = IMG_Load(pathname);
  if(!loaded_surface)
  {
    printf("Unable to load image %s! SDL_image Error: %s\n", pathname, IMG_GetError());
    return NULL;
  }
  //Convert surface to screen format
  optimized_surface = SDL_ConvertSurface(loaded_surface, screen_surface->format, 0);
  if( optimized_surface == NULL )
  {
    printf( "Unable to optimize image %s! SDL Error: %s\n", pathname, SDL_GetError());
  }
  
  //Get rid of old loaded surface
  SDL_FreeSurface(loaded_surface);

  return optimized_surface;
}

int init_game(GameWindow* game_window)
{
  if (!game_window)
    return ERROR_NULL_PARAM;

  // Load modules
  if (SDL_Init(SDL_INIT_VIDEO) < 0)
  {
    printf("SDL_Init Error: %s", SDL_GetError());
    return ERROR_SDL;
  }

  int img_flags = IMG_INIT_PNG;
  if(!(IMG_Init(img_flags) & img_flags))
  {
    printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
    return ERROR_SDL;
  }

  // Create game window
  game_window->window = SDL_CreateWindow("SDL Tutorial",
                                         SDL_WINDOWPOS_UNDEFINED,
                                         SDL_WINDOWPOS_UNDEFINED,
                                         SCREEN_WIDTH,
                                         SCREEN_HEIGHT,
                                         SDL_WINDOW_SHOWN);
  if (!game_window->window)
  {
    printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
    return ERROR_SDL;
  }
  game_window->screen_surface = SDL_GetWindowSurface(game_window->window);

  game_window->renderer = SDL_CreateRenderer(game_window->window, -1, SDL_RENDERER_ACCELERATED );
  if (!game_window->renderer)
  {
    printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError());
    return ERROR_SDL;
  }
  SDL_SetRenderDrawColor(game_window->renderer, 0xFF, 0xFF, 0xFF, 0xFF);
  
  return NO_ERROR;
}

void destroy_game(GameWindow* game_window)
{
  SDL_DestroyRenderer(game_window->renderer);
  SDL_FreeSurface(game_window->screen_surface);
  SDL_DestroyWindow(game_window->window);
  game_window->screen_surface = NULL;
  game_window->window = NULL;
  SDL_Quit();
}
