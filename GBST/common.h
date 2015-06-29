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
#define MAXAUX      5000000
#define MAXITER     5000000

#define TIME_PER_BENCH 10

struct __attribute__((__packed__)) node
{
    struct node *left;
    struct node *right;
    unsigned tid;
    unsigned value;
    unsigned char mark;
};

struct aux_info
{
    unsigned lock;                      //Tree lock
    unsigned op_count;                  //Tree operations for insert&delete
    int count_node;                     //sub-tree node count
    int overflow;                       //child triangle count
    unsigned b_lock;                    //lock for buffer
    unsigned b_count;                   //buffer counter
    struct node *root;
    struct node *mirror;                //Placeholder for mirror tree
    int *buffer;                       //[NUM_THREAD];       //buffer for multiple rearrange

};

struct aux_table
{
    unsigned init_lock;
    unsigned last_tid;
    unsigned alloc_ed;
    struct aux_info *row;               //[MAXINT];
};


struct global
{
	int max_node;
	int max_depth;
	int nodecnt;
    int maxcnt;
    float density;
    float *iratio;
    int count_ins;
    int count_del;
	int failed_ins;
    int failed_del;
	int rebalance_done_ins;
    int rebalance_done_del;
	int merging_done;
    int nb_thread;
    struct node *root;
    struct aux_table aux;
};


#ifdef __cplusplus
extern "C" {
#endif

/* Array Operations */
void print_int_array(const int *array, size_t len);
int int_cmp(const void *a, const void *b);
void sort_all(int* array, long size);
void blank_int_array(int *array, size_t len);
void create_rec( struct node **p, int n, struct node **mine, int *last, int u);


/* Tree Operations */

void printMaxMin(struct global *universe);
void printStat(struct global *universe);
void traverse_all(struct global *universe, int show);
void init_global(struct global *universe);

struct node * smart_it_search( struct node *p, int val);
int insertNode(struct global* universe, int val);
int deleteNode(struct global* universe, int val);
int searchNode(struct global* universe, int val);
int reArrange(struct node *root, struct node *mirror, int* extraBuf, int countBuf, int trsize, int nb_thread, int overflow);
int fill_val_imp( struct node *p , int *array, int* cnt, int *shift, int *lastval, int tdepth, int mindepth, int maxdepth, int maxcount);
void travNode(struct node *p, int lastval, int mark, void (*cb)(void*), long * travcount, long* allcount);


void nop(void *null);
int rand_range_re(unsigned int *seed, long r);
void build_balanced_height(struct global *unv, struct node *p, int height, int start);


#ifdef __cplusplus
}
#endif


#endif
