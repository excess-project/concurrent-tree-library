/*
 common.h

 CBTree - A concurrent B+Tree based on Lehman & Yao paper

 by Ibrahim Umar

 Based on the bpt: B+ Tree implementation

 bpt copyright is as below:

 */


/*
 *  bpt.c
 */
#define Version "1.13"
/*
 *
 *  bpt:  B+ Tree Implementation
 *  Copyright (C) 2010  Amittai Aviram  http://www.amittai.com
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 *  Author:  Amittai Aviram
 *    http://www.amittai.com
 *    amittai.aviram@gmail.edu or afa13@columbia.edu
 *    Senior Software Engineer
 *    MathWorks, Inc.
 *    3 Apple Hill Drive
 *    Natick, MA 01760
 *  Original Date:  26 June 2010
 *  Last modified: 15 April 2014
 *
 *  This implementation demonstrates the B+ tree data structure
 *  for educational purposes, includin insertion, deletion, search, and display
 *  of the search path, the leaves, or the whole tree.
 *
 *  Must be compiled with a C99-compliant C compiler such as the latest GCC.
 *
 *  Usage:  bpt [order]
 *  where order is an optional argument
 *  (integer MIN_ORDER <= order <= MAX_ORDER)
 *  defined as the maximal number of pointers in any node.
 *
 */


#ifndef common_h
#define common_h

#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include "locks.h"

typedef struct node {

        void ** pointers;
        uintptr_t * keys;
        struct node * parent;
        bool is_leaf;
        int num_keys;
        struct node * next; // Used for queue.

        pthread_spinlock_t lock;
        struct node * right_link;
        uintptr_t high_key;

} node;

node** cbtree_alloc();
int search_par(struct node* root, uintptr_t key);
void* get_par(struct node* root, uintptr_t key);

int delete_par(struct node* root, uintptr_t key);
int insert_par(struct node** root, uintptr_t key, void* value);
node * destroy_tree(node * root);

#endif
