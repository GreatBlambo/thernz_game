#include <SDL2/SDL.h>

#include "rw_lock.h"

void RWLock_init(RWLock* rw_lock)
{
  rw_lock->r_lock = SDL_CreateSemaphore(0);
  rw_lock->w_lock = SDL_CreateSemaphore(1);
}

void RWLock_destroy(RWLock* rw_lock)
{
  RWLock_w_lock(rw_lock);
  SDL_DestroySemaphore(rw_lock->r_lock);
  SDL_DestroySemaphore(rw_lock->w_lock);  
}

void RWLock_r_lock(RWLock* rw_lock)
{
  SDL_SemWait(rw_lock->w_lock);
  SDL_SemPost(rw_lock->r_lock);
  SDL_SemPost(rw_lock->w_lock);
}

void RWLock_r_unlock(RWLock* rw_lock)
{
  while (SDL_SemTryWait(rw_lock->r_lock) != 0)
  {
    SDL_Delay(1);
  }
}

void RWLock_w_lock(RWLock* rw_lock)
{
  int readers = 0;
  SDL_SemWait(rw_lock->w_lock);

  while (true)
  {
    if (readers = SDL_SemValue(rw_lock->r_lock) != -1
	&& readers == 0)
      break;
    SDL_Delay(1);
  }
  printf("%i\n", SDL_SemValue(rw_lock->r_lock));
}

void RWLock_w_unlock(RWLock* rw_lock)
{
  SDL_SemPost(rw_lock->w_lock);
}
