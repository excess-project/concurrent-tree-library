/*
 map_select.h

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
 */



#ifndef map_select_h
#define map_select_h

#include <math.h>


#ifdef MAP_USE_CBTREE
#define CBTREE
#endif

#ifdef MAP_USE_DTREE
#define DTREE
#endif

#ifdef MAP_USE_BBST
#define BBST
#endif

#ifdef MAP_USE_GBST
#define GBST
#endif


#include "bench.h"

#define MAP_T data_t

#define MAP_FIND(root,x) 	BENCH_SEARCH(root, x)
#define MAP_CONTAINS(root,x)	BENCH_SEARCH(root, x)
#define MAP_REMOVE(root,x)	BENCH_DELETE(root, x)
#define	MAP_INSERT(root,x)	BENCH_INSERT(root, x)


#ifdef MAP_USE_CBTREE

extern pthread_spinlock_t global_lock;

data_t root = NULL;

data_t cbtree_alloc(int a, int b)
{
	root = malloc(sizeof(void*));
	*root = NULL;

	pthread_spin_init(&global_lock, PTHREAD_PROCESS_PRIVATE);
	return root;
}

#define MAP_ALLOC(x,y) cbtree_alloc(x,y)
#define MAP_FREE(x) {}

#endif

#ifdef MAP_USE_GBST

data_t gbst_alloc(int a, int b)
{
	greenbst_t *greenbstPtr = greenbst_alloc(4095);
	assert(greenbstPtr);
	return greenbstPtr;
}

#define MAP_ALLOC(x,y) gbst_alloc(x,y)
#define MAP_FREE(x) {}

#endif

#ifdef MAP_USE_DTREE

data_t dtree_alloc(int a, int b)
{
	greenbst_t *greenbstPtr = greenbst_alloc();
	assert(greenbstPtr);
	return greenbstPtr;
}

#define MAP_ALLOC(x,y) dtree_alloc(x,y)
#define MAP_FREE(x) {}

#endif

#ifdef MAP_USE_BBST

data_t bbst_alloc(int a, int b)
{

    int ii;
    /* Allocate the universe */

    data_t universe = malloc(sizeof(struct global));

    universe->root = 0;
    universe->failed_ins = 0;
    universe->failed_del = 0;
    universe->count_ins = 0;
    universe->count_del = 0;
    universe->rebalance_done_ins = 0;
    universe->rebalance_done_del = 0;
    universe->nodecnt = 0;                              // Initial node count
    universe->maxcnt = 0;                               // Initial MAXnode count
    universe->density = 0.5;                              // Initial density

    universe->nb_thread = 20;                            // Number of threads

    universe->max_depth =  ceil(log(127)/log(2));         // Maximum tree depth based on the cache line (triangle size)
    universe->max_node  =  (1 << universe->max_depth)-1;// Maximum node for the tree

    universe->iratio = malloc(universe->max_depth * sizeof(float));         //Pre-Compute per-level ratio based on density
    for(ii=0;ii<universe->max_depth;ii++){
        universe->iratio[ii] = universe->density + ii * ((1 - universe->density) / (universe->max_depth - 1));
        //DEBUG_PRINT("level %d ratio: %f\n" , ii, universe->iratio[ii]);
    }

    init_global(universe);

    return universe;
}

#define MAP_ALLOC(x,y) bbst_alloc(x,y)
#define MAP_FREE(x) {}

#endif


#endif
