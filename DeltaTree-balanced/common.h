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

#ifndef hello_common_h
#define hello_common_h

#ifdef DEBUG
#define DEBUG_PRINT(...) {fprintf(stderr, "\n>>DEBUG: ");fprintf( stderr, __VA_ARGS__ ); }
#else
#define DEBUG_PRINT(...) {}
#endif

#define MAXINT  INT_MAX
#define EMPTY   0
#define FULL    1

#define PRE_ALLOC   1
#define MAXITER     5000000 //5000000
#define MAX_STACK   100

#define TIME_PER_BENCH 10

static inline
unsigned _val(unsigned x) {
    return x & ~(1<<31);
}

static inline
void set_mark(unsigned *value)
{
    *value = *value | 1<<31;
}

static inline
void unset_mark(unsigned *value)
{
    *value = *value & ~(1<<31);
}

static inline
unsigned is_marked(unsigned value)
{
    return (value >> 31);
}

struct map{short left; short right; short parent;};

extern struct map *_map;

struct __attribute__((__packed__)) node_ref
{
    struct node_ref *left;
    struct node_ref *right;
    unsigned tid;
    unsigned value;
    unsigned char mark;
};

struct __attribute__((__packed__)) node
{
    unsigned value;
};


struct aux_info
{
    volatile int lock;                      //Tree lock
    int count_node;                     //sub-tree node count
};

struct deltaNode
{
    struct node* a;
    struct deltaNode** b;
    struct aux_info* info;
    
    int high_key;
    struct deltaNode *sibling;
    char isleaf;
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
	int nb_thread;
    volatile int lock;
    struct deltaNode **root;
    struct node_ref *ref;
};

struct stack{
    int num;
    struct deltaNode* parent[MAX_STACK];
};

int push(struct stack *stk, void* parent);
struct deltaNode* pop(struct stack *stk);


typedef int indext;

extern char *optarg;
extern int optind, opterr, optopt;
typedef int domain;
volatile domain res;

/* Array Operations */
void print_int_array(const int *array, size_t len);
int int_cmp(const void *a, const void *b);
void sort_all(int* array, long size);
void blank_int_array(int *array, size_t len);
void create_rec( struct node_ref **p, int n, struct node_ref **mine, int *last, int u);

inline void* left(void* p, void* base, size_t nodesize);
inline void* right(void* p, void* base, size_t nodesize);

/* Tree Operations */
int insert_par( struct global *universe, int key);
int insertNode_lo(struct global* universe, int val);
int searchNode_lo(struct global* universe, int val);
int deleteNode_lo(struct global* universe, int val);


void init_global(struct global *universe);
void create_map( struct node_ref *p, int size );
void initial_add (struct global *universe, int num, int range);

void report_all( struct node *p );

int rand_range_re(unsigned int *seed, long r);
void printStat(struct global *universe);

void nop(void *null);


#endif
