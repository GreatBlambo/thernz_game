#include <SDL2/SDL_mutex.h>

struct RWLock
{
  SDL_sem* r_lock;
  SDL_sem* w_lock;
};

void RWLock_init(RWLock* rwlock);
void RWLock_destroy(RWLock* rwlock);
void RWLock_w_lock(RWLock* rwlock);
void RWLock_w_unlock(RWLock* rwlock);
void RWLock_r_lock(RWLock* rwlock);
void RWLock_r_unlock(RWLock* rwlock);

