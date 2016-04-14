/*
 bench.h

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


#ifndef bench_h
#define bench_h

#ifdef SVEB //Static implicit veb

#include "../SVEB/staticvebtree.h"

#define data_t domain*

#define BENCH_SEARCH(root, x)  search_test(x)
#define BENCH_DELETE(root, x)  delete_node(x)
#define BENCH_INSERT(root, x)  insert(x)

#endif


#ifdef VEB //Static pointer veb

#include "../VEB/veb.h"

#define data_t struct node*

#define BENCH_SEARCH(root, x)  smart_it_search(root, x)
#define BENCH_DELETE(root, x)  smart_it_search(root, x)
#define BENCH_INSERT(root, x)  searchNode(root, x)

#endif

#ifdef BSTTK //Portably scalable tree (ASPLOS'15)

#include "../BSTTK/bst_tk.h"

#define data_t intset_t*

#define BENCH_SEARCH(root, x)  bst_tk_find(root, x)
#define BENCH_DELETE(root, x)  bst_tk_delete(root, x)
#define BENCH_INSERT(root, x)  bst_tk_insert(root, x, x)

#endif


#ifdef CBTREE //Concurrent B+tree (Blink)

#include "../CBTree/common.h"

#define data_t struct node**

#define BENCH_SEARCH(root, x)  search_par(*root, x)
#define BENCH_DELETE(root, x)  delete_par(*root, x)
#define BENCH_INSERT(root, x)  insert_par(root, x, x)

#endif

#ifdef DTREE

#include "../DeltaTree/dtree.h"

#define data_t struct global*

#define BENCH_SEARCH(root, x)  searchNode_lo(root, x)
#define BENCH_DELETE(root, x)  deleteNode_lo(root, x)
#define BENCH_INSERT(root, x)  insert_par(root, x, 0)

#endif


#ifdef GBST

#include "../GreenBST/gbst.h"

#define data_t struct global*

#define BENCH_SEARCH(root, x)  greenbst_contains(root, x)
#define BENCH_DELETE(root, x)  greenbst_delete(root, x)
#define BENCH_INSERT(root, x)  greenbst_insert(root, x, 0)

#endif

#ifdef BBST

#include "../BlueBST/tree.h"

#define data_t struct global*

#define BENCH_SEARCH(root, x)  searchNode(root, x)
#define BENCH_DELETE(root, x)  deleteNode(root, x)
#define BENCH_INSERT(root, x)  insertNode(root, x)

#endif

#ifdef LFBST

#include "../LFBST/wfrbt.h"
#include "../LFBST/operations.h"


#define data_t thread_data_t*

#define BENCH_SEARCH(root, x)  search(root, x)
#define BENCH_DELETE(root, x)  delete_node(root, x)
#define BENCH_INSERT(root, x)  insert(root, x)

#endif

#ifdef RCUT

#include "../citrus/citrus.h"
#include "../citrus/urcu.h"

#define data_t node

#define BENCH_SEARCH(root, x)  contains(root, x)
#define BENCH_DELETE(root, x)  delete_node(root, x)
#define BENCH_INSERT(root, x)  insert(root, x, x)

#endif

void start_benchmark(data_t, int, int , int, int);
void testseq(data_t, int);
void testpar(data_t, int, int, int);

#endif
