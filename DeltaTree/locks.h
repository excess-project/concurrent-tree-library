/*
 DeltaTree
 
 Copyright 2013 Ibrahim Umar, University of Tromsø.
 
 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at
 
 http://www.apache.org/licenses/LICENSE-2.0
 
 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 
 ---
 
 Several vEB related benchmarks & operations are based on the code from:
 G. S. Brodal, R. Fagerberg, and R. Jacob, “Cache oblivious search trees via binary trees of small height,”
 in Proceedings of the thirteenth annual ACM-SIAM symposium on Discrete algorithms, ser. SODA ’02, 2002, pp. 39–48.
 
 */

#ifndef DeltaTree_locks_h
#define DeltaTree_locks_h

#include<pthread.h>

//LOCKS credits: http://locklessinc.com/articles/locks/
#define EBUSY 1
#define EFREE 0

#define atomic_xadd(P, V) __sync_fetch_and_add((P), (V))
#define cmpxchg(P, O, N) __sync_val_compare_and_swap((P), (O), (N))
#define atomic_inc(P) __sync_add_and_fetch((P), 1)
#define atomic_dec(P) __sync_add_and_fetch((P), -1)
#define atomic_add(P, V) __sync_add_and_fetch((P), (V))
#define atomic_set_bit(P, V) __sync_or_and_fetch((P), 1<<(V))

/* Compile read-write barrier */
#define barrier() asm volatile("": : :"memory")

/* Pause instruction to prevent excess processor bus usage */
#define cpu_relax() asm volatile("pause\n": : :"memory")

/* Atomic exchange (of various sizes) */

static inline void *xchg_64(void *ptr, void *x)
{
	__asm__ __volatile__("xchgq %0,%1"
                         :"=r" (x)
                         :"m" (*(volatile long long *)ptr), "0" ((unsigned long long) x)
                         :"memory");
    
	return x;
}

static inline unsigned xchg_32(void *ptr, unsigned x)
{
	__asm__ __volatile__("xchgl %0,%1"
                         :"=r" (x)
                         :"m" (*(volatile unsigned *)ptr), "0" (x)
                         :"memory");
    
	return x;
}

static inline unsigned short xchg_16(void *ptr, unsigned short x)
{
	__asm__ __volatile__("xchgw %0,%1"
                         :"=r" (x)
                         :"m" (*(volatile unsigned short *)ptr), "0" (x)
                         :"memory");
    
	return x;
}

/* Test and set a bit */
static inline char atomic_bitsetandtest(void *ptr, int x)
{
	char out;
	__asm__ __volatile__("lock; bts %2,%1\n"
                         "sbb %0,%0\n"
                         :"=r" (out), "=m" (*(volatile long long *)ptr)
                         :"Ir" (x)
                         :"memory");
    
	return out;
}

static inline void flag_up(unsigned *p){
    atomic_inc(p);
}

static inline void flag_down(unsigned *p){
    atomic_dec(p);
    /*
    if(*p > NUM_THREAD){
        printf("ERROR: Ops flag is: %u\n", *p);
        exit(0);
    }
    */
}

static inline void spin_wait(unsigned *p){
    while (1)
	{
		if (*p == EFREE) return;
        
		while (*p) cpu_relax();
	}
}

static inline void spin_lock(unsigned *lock)
{
	while (1)
	{
		if (!xchg_32(lock, EBUSY)) return;
        
		while (*lock) cpu_relax();
	}
}

static inline unsigned spin_wait_counting(unsigned *p)
{
	unsigned has_count=0;
    while (1)
	{
		if (*p == EFREE) return has_count;
        
		while (*p) cpu_relax();
        has_count=1;
	}
}


static inline void spin_unlock(unsigned *lock)
{
	barrier();
	*lock = 0;
}

static inline int spin_trylock(unsigned *lock)
{
	return xchg_32(lock, EBUSY);
}

/*
 static inline void spin_lock(unsigned *p)
 {
 while(!__sync_bool_compare_and_swap(p, 0, 1))
 {
 cpu_relax(); // for gcc/IA-32
 }
 }
 
 static inline void spin_unlock(unsigned *p)
 {
 barrier();
 *p = 0;
 }
 */

static inline void wait_and_check(unsigned *lock, unsigned *flag){
    int repeat;
    do{
        spin_wait(lock);
        /*
        atomic_inc(&entering_top);
        if(spin_wait_counting(lock))
            atomic_inc(&waiting_top);
        */
        flag_up(flag);
        repeat = 0;
        if(*lock){
            flag_down(flag);
            repeat = 1;
        }
    }while(repeat);
}

#ifdef __APPLE__

#ifndef PTHREAD_BARRIER_H_
#define PTHREAD_BARRIER_H_

#include <pthread.h>
#include <errno.h>

typedef int pthread_barrierattr_t;
typedef struct
{
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int count;
    int tripCount;
} pthread_barrier_t;


inline int pthread_barrier_init(pthread_barrier_t *barrier, const pthread_barrierattr_t *attr, unsigned int count)
{
    if(count == 0)
    {
        errno = EINVAL;
        return -1;
    }
    if(pthread_mutex_init(&barrier->mutex, 0) < 0)
    {
        return -1;
    }
    if(pthread_cond_init(&barrier->cond, 0) < 0)
    {
        pthread_mutex_destroy(&barrier->mutex);
        return -1;
    }
    barrier->tripCount = count;
    barrier->count = 0;
    
    return 0;
}

inline int pthread_barrier_destroy(pthread_barrier_t *barrier)
{
    pthread_cond_destroy(&barrier->cond);
    pthread_mutex_destroy(&barrier->mutex);
    return 0;
}

inline int pthread_barrier_wait(pthread_barrier_t *barrier)
{
    pthread_mutex_lock(&barrier->mutex);
    ++(barrier->count);
    if(barrier->count >= barrier->tripCount)
    {
        barrier->count = 0;
        pthread_cond_broadcast(&barrier->cond);
        pthread_mutex_unlock(&barrier->mutex);
        return 1;
    }
    else
    {
        pthread_cond_wait(&barrier->cond, &(barrier->mutex));
        pthread_mutex_unlock(&barrier->mutex);
        return 0;
    }
}

#endif // PTHREAD_BARRIER_H_
#endif // __APPLE__

#endif //locks.h
