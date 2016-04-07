#ifndef locks_h
#define locks_h

#if  defined(__APPLE__)

#include <libkern/OSAtomic.h>

typedef volatile OSSpinLock pthread_spinlock_t;

#ifndef PTHREAD_PROCESS_SHARED
# define PTHREAD_PROCESS_SHARED 1
#endif
#ifndef PTHREAD_PROCESS_PRIVATE
# define PTHREAD_PROCESS_PRIVATE 2
#endif

int pthread_spin_init(pthread_spinlock_t *lock, int pshared);
int pthread_spin_destroy(pthread_spinlock_t *lock);
int pthread_spin_lock(pthread_spinlock_t *lock);
int pthread_spin_trylock(pthread_spinlock_t *lock);
int pthread_spin_unlock(pthread_spinlock_t *lock);

#endif

#if  defined(__SIM) && !defined(pthread_spinlock_t)

#include "spinlock_x86.h"

typedef volatile int pthread_spinlock_t;

#ifndef PTHREAD_PROCESS_SHARED
# define PTHREAD_PROCESS_SHARED 1
#endif
#ifndef PTHREAD_PROCESS_PRIVATE
# define PTHREAD_PROCESS_PRIVATE 2
#endif

#define  pthread_spin_init(X, Y) 	{*X=0;}
#define  pthread_spin_destroy(X) 	{*X=0;}
#define  pthread_spin_lock(X)		spin_lock(X)
#define  pthread_spin_trylock(X)	!trylock(X)
#define  pthread_spin_unlock(X)		spin_unlock(X)

#endif

#endif
