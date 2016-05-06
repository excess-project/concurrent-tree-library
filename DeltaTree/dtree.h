/*
 deltatree.h

 DeltaTree

 This is part of the tree library

 Copyright 2015 Ibrahim Umar (UiT the Arctic University of Norway)

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

 Several vEB related operations are based on the code from:
 G. S. Brodal, R. Fagerberg, and R. Jacob, “Cache oblivious search trees via binary trees of small height,”
 in Proceedings of the thirteenth annual ACM-SIAM symposium on Discrete algorithms, ser. SODA ’02, 2002, pp. 39–48.
 
 */



#ifndef gbst_h
#define gbst_h


#include "locks.h"

#include <pthread.h>

#define atomic_inc(P) __sync_add_and_fetch((P), 1)
#define atomic_dec(P) __sync_add_and_fetch((P), -1)

//To count the number of waiting thread
//#define __WAIT_COUNT

#ifdef __WAIT_COUNT

unsigned __wait;

static inline int pthread_spin_lock_w(pthread_spinlock_t *lock) {
    if(pthread_spin_trylock(lock))
    {
        atomic_inc(&__wait);
        pthread_spin_lock(lock);
    }
    return 0;
}

#else

#define pthread_spin_lock_w(x)  pthread_spin_lock(x)

#endif /* __WAIT_COUNT */


#ifdef DEBUG
#define DEBUG_PRINT(...) {fprintf(stderr, "\n>>DEBUG: ");fprintf( stderr, __VA_ARGS__ ); }
#else
#define DEBUG_PRINT(...) {}
#endif

#define MAXINT  INT_MAX
#define EMPTY   0
#define FULL    1

//#define calloc(...) exit(0);


#define GNODES_SIZE 1023

#define MAX_STACK 100

#define MAX_POOLSIZE 150000

struct map{short left; short right; short parent;};

struct node_ref
{
    struct node_ref *left;
    struct node_ref *right;
};

struct node
{
    unsigned value;
};

struct aux_info
{
    pthread_spinlock_t lock;        //Tree lock
    int count_node;           //sub-tree node count
    int deleted_node;
};

struct deltaNode
{
	unsigned rev;
    unsigned high_key;
    struct deltaNode *sibling;
    char isleaf;
    
    struct aux_info* info;
    struct node* a;
    void** b;
};

struct global
{
	int max_node;
	int max_depth;
	int nodecnt;
    int maxcnt;
    
    int split_thres;
    
    int count_ins;
    int count_del;
	int failed_ins;
    int failed_del;
	int rebalance_done_ins;
    int rebalance_done_del;
    pthread_spinlock_t lock;
    struct deltaNode **root;
    struct node_ref *ref;
};

struct stack{
    int num;
    struct deltaNode* parent[MAX_STACK];
};
static inline
int push(struct stack *stk, void* parent){
    stk->parent[stk->num++] = parent;
    return 0;
}
static inline
struct deltaNode* pop(struct stack *stk){
    if(stk->num > 0){
        stk->num--;
        return stk->parent[stk->num];
    }else{
        return 0;
    }
}

/* Externs */

extern struct node_ref minebuf[GNODES_SIZE];

//Extern for Map
extern struct map mapcontent[GNODES_SIZE];
extern struct map *_map;
 
//Extern for Pool
extern struct pool poolrepo;
extern struct pool *_pool;
 
//Extern for Counter
extern unsigned poolCounter;
extern unsigned  *_poolCtr;


typedef struct global deltatree_t;

deltatree_t * deltatree_alloc();
int deltatree_free(deltatree_t *);

int deltatree_contains(deltatree_t *, unsigned);
void* deltatree_get(deltatree_t *, unsigned);
int deltatree_insert(deltatree_t *, unsigned, void*);
int deltatree_delete(deltatree_t *, unsigned);
void printStat(struct global *universe);

void initial_add (struct global *universe, int num, int range);

struct pool{
    struct deltaNode deltaNodepool[MAX_POOLSIZE];
    struct node nodepool[MAX_POOLSIZE][GNODES_SIZE];
    size_t linkpool [MAX_POOLSIZE][GNODES_SIZE];
    struct aux_info auxpool[MAX_POOLSIZE];
};

#endif /* gbst_h */
