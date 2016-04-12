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

#ifdef MAP_USE_CBTREE
#define CBTREE
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


#endif
