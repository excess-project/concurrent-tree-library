//
//  gbstlock.h
//
//
//  Created by Ibrahim Umar on 12/02/16.
//
//

#ifndef gbstlock_h
#define gbstlock_h

#include <pthread.h>
#include "locks.h"


#ifdef __USEMUTEX

#define gbst_lock_t pthread_mutex_t

#else

#define gbst_lock_t pthread_spinlock_t

#endif

int gbst_lock_init(gbst_lock_t *lock);

int gbst_lock(gbst_lock_t *lock);

int gbst_unlock(gbst_lock_t *lock);


#endif /* gbstlock_h */
