#if  defined(__APPLE__)

#include <libkern/OSAtomic.h>
#include "locks.h"

int pthread_spin_init(pthread_spinlock_t *lock, int pshared) {
	*lock = OS_SPINLOCK_INIT;
	return 0;
}

int pthread_spin_destroy(pthread_spinlock_t *lock) {
	return 0;
}

int pthread_spin_lock(pthread_spinlock_t *lock) {
	OSSpinLockLock(lock);
    return 0;
}

int pthread_spin_trylock(pthread_spinlock_t *lock) {
	return !OSSpinLockTry(lock);
}

int pthread_spin_unlock(pthread_spinlock_t *lock) {
	OSSpinLockUnlock(lock);
	return 0;
}

#endif
