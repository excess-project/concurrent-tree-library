//
//  gbstlock.h
//
//
//
//

#include <pthread.h>
#include <stdio.h>
#include "gbstlock.h"
#include "locks.h"

#ifdef __USEMUTEX

int gbst_lock_init(gbst_lock_t *lock)
{
	int ret;
	pthread_mutexattr_t Attr;

	pthread_mutexattr_init(&Attr);
	pthread_mutexattr_settype(&Attr, PTHREAD_MUTEX_ERRORCHECK);

	ret = pthread_mutex_init(lock, &Attr);

	if (ret != 0) {
		fprintf(stderr, "Mutex init failed %d!\n", ret);
		return 1;
	} else {
		return 0;
	}
}

int gbst_lock(gbst_lock_t *lock)
{
	return pthread_mutex_lock(lock);
}

int gbst_unlock(gbst_lock_t *lock)
{
	return pthread_mutex_unlock(lock);
}

#else

int gbst_lock_init(gbst_lock_t *lock)
{
	int ret;

	ret = pthread_spin_init(lock, PTHREAD_PROCESS_SHARED);
	if (ret != 0) {
		fprintf(stderr, "Spinlock init failed %d!\n", ret);
		return 1;
	} else {
		return 0;
	}
}

int gbst_lock(gbst_lock_t *lock)
{
	return pthread_spin_lock(lock);
}

int gbst_unlock(gbst_lock_t *lock)
{
	return pthread_spin_unlock(lock);
}

#endif
