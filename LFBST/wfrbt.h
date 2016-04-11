#ifndef wfrbt_h
#define wfrbt_h

#include <algorithm>
#include <chrono>
#include <iostream>
#include <fstream>
#include <pthread.h>
#include <setjmp.h>
#include <stdint.h>
#include <unistd.h>
#include <vector>

#include "atomic_ops.h"
#include "atomic_ops/sysdeps/standard_ao_double_t.h"

#define RECYCLED_VECTOR_RESERVE 5000000

#define MARK_BIT 1
#define FLAG_BIT 0

enum {INS,DEL};
enum {UNMARK,MARK};
enum {UNFLAG,FLAG};

typedef uintptr_t Word;

typedef struct node{
	int key;
	volatile AO_double_t child;
#ifdef UPDATE_VAL
	long value;
#endif
} node_t;

typedef struct seekRecord{
  // SeekRecord structure
  size_t leafKey;
  node_t * parent;
  AO_t pL;
  bool isLeftL; // is L the left child of P?
  node_t * lum;
  AO_t lumC;
  bool isLeftUM; // is  last unmarked node's child on access path the left child of  the last unmarked node?
} seekRecord_t;

typedef uintptr_t val_t;

typedef struct thread_data {
  unsigned long nb_added;
  unsigned long nb_removed;
  node_t* rootOfTree;
  std::vector<node_t *> recycledNodes;
  seekRecord_t * sr; // seek record
  seekRecord_t * ssr; // secondary seek record

} thread_data_t;


inline void *xmalloc(size_t size) {
  void *p = malloc(size);
  if (p == NULL) {
    perror("malloc");
    exit(1);
  }
  return p;
}


// Forward declaration of window transactions
int perform_one_delete_window_operation(thread_data_t* data, seekRecord_t * R, size_t key);

int perform_one_insert_window_operation(thread_data_t* data, seekRecord_t * R, size_t newKey);


/* ################################################################### *
 * Macro Definitions
 * ################################################################### */





#define atomic_cas_full(addr, old_val, new_val) __sync_bool_compare_and_swap(addr, old_val, new_val);


//-------------------------------------------------------------
#define create_child_word(addr, mark, flag) (((uintptr_t) addr << 2) + (mark << 1) + (flag))
#define is_marked(x) ( ((x >> 1) & 1)  == 1 ? true:false)
#define is_flagged(x) ( (x & 1 )  == 1 ? true:false)

#define get_addr(x) (x >> 2)
#define add_mark_bit(x) (x + 4UL)
#define is_free(x) (((x) & 3) == 0? true:false)

//-------------------------------------------------------------


#endif
