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

#define _GNU_SOURCE
#include <sched.h>

#include <unistd.h>

#include<stdio.h>
#include<stdlib.h>
#include<getopt.h>
#include<math.h>
#include<search.h>

#include<time.h>
#include<sys/times.h>
#include<sys/time.h>

#include<signal.h>
#include<limits.h>

#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<sys/mman.h>

#include <inttypes.h>
#include<pthread.h>

#include "common.h"
#include "bench.h"


int main(int argc, char **argv ) {
    int myopt = 0;
    struct global *universe = 0;
    
    float  d;
    int s, u, n, i, t, r, v;       //Various parameters
    
    
    //myname = argv[0];
    
    i = 127;            //default initial element count
    t = 1023;           //default triangle size
    r = 5000000;        //default range size
    u = 10;             //default update rate
    s = 0;              //default seed
    n = 1;              //default number of thread
    d = (float)1/2;     //default density
    
    v=0;                //default valgrind mode (reduce stats)
    
    fprintf(stderr,"\nDeltaTree v0.1\n===============\n\n");
    if(argc < 2)
        fprintf(stderr,"NOTE: No parameters supplied, will continue with defaults\n");
    fprintf(stderr,"Use -h switch for help.\n\n");
    
    while( EOF != myopt ) {
        myopt = getopt(argc,argv,"r:t:n:i:u:s:d:v:hb:");
        switch( myopt ) {
            case 'r': r = atoi( optarg ); break;
            case 'n': n = atoi( optarg ); break;
            case 't': t = atoi( optarg ); break;
            case 'i': i = atoi( optarg ); break;
            case 'u': u = atoi( optarg ); break;
            case 's': s = atoi( optarg ); break;
            case 'd': d = atof( optarg ); break;
            case 'v': v = atof( optarg ); break;
            case 'h': fprintf(stderr,"Accepted parameters\n");
                fprintf(stderr,"-r <NUM>    : Range size\n");
                fprintf(stderr,"-u <0..100> : Update ratio. 0 = Only search; 100 = Only updates\n");
                fprintf(stderr,"-i <NUM>    : Initial tree size (inital pre-filled element count)\n");
                fprintf(stderr,"-t <NUM>    : DeltaNode size\n");
                fprintf(stderr,"-n <NUM>    : Number of threads\n");
                fprintf(stderr,"-s <NUM>    : Random seed. 0 = using time as seed\n");
                fprintf(stderr,"-d <0..1>   : Density (in float)\n");
                fprintf(stderr,"-v <0 or 1> : Valgrind mode (less stats). 0 = False; 1 = True\n");
                fprintf(stderr,"-h          : This help\n\n");
                fprintf(stderr,"Benchmark output format: \n\"0: range, insert ratio, delete ratio, #threads, attempted insert, attempted delete, attempted search, effective insert, effective delete, effective search, time (in msec)\"\n\n");
                exit(0);
        }
    }
    fprintf(stderr,"Parameters:\n");
    fprintf(stderr,"- Range size r:\t\t %d\n", r);
    fprintf(stderr,"- DeltaNode size t:\t %d\n", t);
    fprintf(stderr,"- Update rate u:\t %d%% \n", u);
    fprintf(stderr,"- Number of threads n:\t %d\n", n);
    fprintf(stderr,"- Initial tree size i:\t %d\n", i);
    fprintf(stderr,"- Random seed s:\t %d\n", s);
    fprintf(stderr,"- Density d:\t\t %f\n", d);
    fprintf(stderr,"- Valgrind mode v:\t %d\n\n", v);
    
    if (s == 0)
		srand((int)time(0));
	else
		srand(s);
    
    /* Allocate the universe */
    universe = malloc(sizeof(struct global));
    
    universe->root = 0;
    universe->failed_ins = 0;
    universe->failed_del = 0;
    universe->count_ins = 0;
    universe->count_del = 0;
    universe->rebalance_done_ins = 0;
    universe->rebalance_done_del = 0;
    universe->nodecnt = 0;                                          // Initial node count
    universe->maxcnt = 0;                                           // Initial MAXnode count
    
    universe->nb_thread = n;                                        // Number of threads
    
    universe->max_depth =  ceil(log(t)/log(2));                     // Maximum tree depth based on the cache line (triangle size)
    universe->max_node  =  (1 << universe->max_depth)-1;            // Maximum node for the tree
    
    universe->split_thres = universe->max_node >> 2;
    
    
    //universe->iratio = malloc(universe->max_depth * sizeof(float)); // Pre-Compute per-level ratio based on density
    
    //for(ii=0;ii<universe->max_depth;ii++){
    //    universe->iratio[ii] = universe->density + ii * ((1 - universe->density) / (universe->max_depth - 1));
    //    DEBUG_PRINT("level %d ratio: %f\n" , ii, universe->iratio[ii]);
    //}
    
    init_global(universe);
    
    fprintf(stderr,"Finished building initial DeltaTree\n");
    fprintf(stderr, "The node size is: %ld bytes\n", sizeof(struct node));
    
    create_map(universe->ref, universe->max_node); //Now lets make the map
    
    //testseq(universe);
    //exit(0);
    //report_all(universe->root);
    //srand(0);
    //untest = universe;
    //test(i, u, n, v, 0);
    //printStat(universe);
    //exit(0);
    
    if(i){
        fprintf(stderr,"Now pre-filling %d random elements...\n", i);
        initial_add(universe, i, r);
    }
    
    fprintf(stderr, "Finished init a DeltaTree using DeltaNode size %d, with initial %d members\n", universe->max_node, i);
    fflush(stderr);
    
    start_benchmark(universe, r, u, n, v);
    
    exit(0);
    //report_all_ref(universe->ref);
    
    /*
    struct timeval st,ed;
    
    int iter = 1000;
    int abc = 0, bcd = 0;
    struct node *temp;
    
    
    r = 1023;
    
    gettimeofday(&st, NULL);
    for(abc = 0; abc <= iter; abc++){
        for(bcd = 1; bcd <= r; bcd++)
            insertNode(universe, bcd);
        
        //report_all(universe->root);
        
        for(bcd = 1; bcd <= r; bcd++){
            temp = smart_it_search(universe->root->a, universe->root->a, bcd);
            //fprintf(stderr, "%d == %d, %d\n", bcd, _val(temp->value), is_marked(temp->value));
            if(bcd != temp->value)
                exit(EXIT_FAILURE);
        }
        for(bcd = 1; bcd <= r; bcd++)
            deleteNode(universe, bcd);
        
        //report_all(universe->root);
        
        for(bcd = 0; bcd < r; bcd++){
            universe->root->a[bcd].value = EMPTY;
        }
    }
        gettimeofday(&ed, NULL);
        
        printf("s : %lu usec\n", (ed.tv_sec - st.tv_sec)*1000000 + ed.tv_usec - st.tv_usec);
        //printf("ns: %d\n",  ed.tv_usec - st.tv_usec);
        
        printStat(universe);
        //printMaxMin(universe);
       // exit(EXIT_SUCCESS);
    
    */
    int abc;
    int count = 0;
    unsigned val;
    //report_all(universe->root->a);
    printStat(universe);
    
    srand(1);
    for(abc = 0; abc <= 5000; abc++){
        val = (rand()%500000) + 1;
        val = abc + 1;
        fprintf(stderr,"inserting:%d\n", val);
        if(insertNode_lo(universe, val))
            count++;
    }
    
    fprintf(stderr, "\nNum ins: %d\n", count);
    
    //report_all(universe->root->a);
    printStat(universe);
    
    int error = 0;
    
    //struct stack stk;
    
    //report_all(universe->root->a);
    srand(1);
    for(abc = 0; abc <= 5000; abc++){
        val = (rand()%500000) + 1;
        val = abc + 1;
        fprintf(stderr,"searching:%d\n", val);
        //struct deltaNode *dt = smart_btree_search_lo(universe->root, val, universe->max_depth);
        //report_all(dt->a);
        //temp = smart_it_search_lo(dt->a, dt->a, val);
        if(!searchNode_lo(universe, val)){
            error++;
            fprintf(stderr,"err:%d\n", val);
        }
    }
    
    fprintf(stderr, "num err: %d\n", error);
    
    srand(1);
    count = 0;
    for(abc = 0; abc <= 5000; abc++){
        val = (rand()%500000) + 1;
        val = abc + 1;
        fprintf(stderr,"deleting:%d\n", val);
        if(deleteNode_lo(universe, val))
            count++;
    }
    
    fprintf(stderr, "\nNum del: %d\n", count);
    
    srand(1);
    error = 0;
    count = 0;
    for(abc = 0; abc <= 5000; abc++){
        val = (rand()%500000) + 1;
        val = abc + 1;
        fprintf(stderr,"searching:%d\n", val);
        //struct deltaNode *dt = smart_btree_search_lo(universe->root, val, universe->max_depth);
        //report_all(dt->a);
        //temp = smart_it_search_lo(dt->a, dt->a, val);
        if(!searchNode_lo(universe, val)){
            error++;
        }
    }
    
    fprintf(stderr, "num err: %d\n", error);
    return 0;
}


/* NON - LEAF ORIENTED PROCEDURES
 
 void fill_buf(struct node *p, void* base, int* array, unsigned *counter){
 if(p && p->value != EMPTY){
 fill_buf(left(p, base, sizeof(struct node)), base, array, counter);
 if(!is_marked(p->value)){
 array[(*counter)++] = p->value;
 p->value = EMPTY;
 }
 fill_buf(right(p,  base, sizeof(struct node)), base, array, counter);
 }
 }
 
 void smart_fill_val(struct node *p, void* base, int* buf, int l, int r){
 
 unsigned mid = 0;
 
 if(p == NULL || r < l) return;
 
 mid = (int)(l+r) >> 1;
 
 p->value = buf[mid];
 
 //fprintf(stderr, "Val: %d, left: %d, mid: %d, right: %d\n", buf[mid], l, mid, r);
 
 smart_fill_val(left(p, base, sizeof(struct node)), base, buf, l, mid - 1);
 
 smart_fill_val(right(p, base, sizeof(struct node)), base, buf, mid + 1, r);
 
 }
 
 
 int smart_rebalance(struct deltaNode *node, int max_node){
 unsigned count = 0, mid = 0;
 int *buf    = calloc(max_node, sizeof(int));
 
 fill_buf(node->a, node->a, buf, &count);
 
 //memset(universe->root, 0, universe->max_node * sizeof(struct node));
 
 mid = (int)count/2;
 
 node->a->value = buf[mid];
 
 smart_fill_val(left(node->a, node->a, sizeof(struct node)), node->a, buf, 0, mid-1);
 smart_fill_val(right(node->a, node->a, sizeof(struct node)), node->a, buf, mid+1, count);
 
 //report_all(universe->root);
 
 free(buf);
 return 0;
 };
 
 
 
 int insertNode(struct global* universe, int val)
 {
 
 int success, caldep, rootsuccess;
 
 rootsuccess = 0;
 success = 1;
 caldep = 0;
 
 struct aux_info* aux;
 struct node* root;
 struct deltaNode ** link;
 
 root = universe->root->a;
 link = universe->root->b;
 aux = universe->root->info;
 
 
 DEBUG_PRINT("Inserting: %ld", val);
 
 if(universe->root == 0 )
 return 0;           //Invalid
 else if(root->value == EMPTY){
 wait_and_check(&aux->lock, &aux->op_count);
 if(cmpxchg(&root->value, EMPTY, val) != EMPTY){
 success = 0;
 }else{
 aux->count_node++;
 rootsuccess = 1;
 
 }
 flag_down(&aux->op_count);
 }
 if(!rootsuccess){
 //Proceed now, if not the first value
 wait_and_check(&aux->lock, &aux->op_count);
 
 struct node * temp = smart_it_search(root, root, val);
 
 int conti = 1, reb = 0;
 while (conti){
 if(!temp) {conti = 0; success = 0;}
 else{
 if(temp->value == val){
 conti = 0;
 success = 0;
 }else{
 if( val < _val(temp->value)){
 temp = left(temp, root, sizeof(struct node));
 if(temp && cmpxchg(&temp->value, EMPTY, val) == EMPTY){
 conti = 0;
 if(left(temp, root, sizeof(struct node)) == 0) reb = 1;
 }else
 temp = smart_it_search(temp, root, val);
 }else if( val > _val(temp->value)){
 temp = right(temp, root, sizeof(struct node));
 if(temp && cmpxchg(&temp->value, EMPTY, val) == EMPTY){
 conti = 0;
 if(left(temp, root, sizeof(struct node)) == 0) reb = 1;
 }else
 temp = smart_it_search(temp, root, val);
 }else{
 if(is_marked(temp->value)==1)
 unset_mark(&temp->value);
 else
 success = 0;
 conti = 0;
 }
 
 }
 }
 }
 flag_down(&aux->op_count);
 if(reb){
 smart_rebalance(universe->root, universe->max_node);
 atomic_inc(&universe->rebalance_done_ins);
 }
 }
 //use atomic update for stats below!!!
 if(success){
 atomic_inc(&universe->nodecnt);
 if(success != 2)
 atomic_inc(&universe->maxcnt);
 atomic_inc(&universe->count_ins);
 }else{
 atomic_inc(&universe->failed_ins);
 }
 
 #ifdef DEBUG
 if(success){
 DEBUG_PRINT("--Value %ld inserted at: %d\n", val, caldep);
 }else{
 DEBUG_PRINT("--Failed inserting: %ld, reason %d\n", val, success);}
 #endif
 
 return success;
 }
 
 int deleteNode(struct global *universe, int val)
 {
 int success;
 
 struct aux_info* aux;
 struct node* root;
 struct deltaNode ** link;
 
 root = universe->root->a;
 link = universe->root->b;
 aux = universe->root->info;
 
 
 success = 1;
 
 DEBUG_PRINT("Deleting: %ld",val);
 
 if(universe->root == 0)
 success =  0; //Invalid
 else{
 wait_and_check(&aux->lock, &aux->op_count);
 //deleteHelper(universe->aux.row[0].root, val, currID, &success, &merging_start, &universe, 0);
 
 struct node * temp = smart_it_search(root, root, val);
 
 if(val == temp->value){
 if(!cmpxchg(&temp->value, temp->value, (temp->value | 1<<31)) == temp->value) success = 0;
 }else
 success = 0;
 }
 
 if(success){
 atomic_dec(&universe->nodecnt);
 atomic_inc(&universe->count_del);
 
 // Now rebalance if neccessary
 //if(universe->nodecnt && universe->maxcnt/universe->nodecnt > 2) {
 //    rebalance(universe);
 //    universe->maxcnt = universe->nodecnt;
 //atomic_inc(&universe->rebalance_done_del);
 //}
 }else{
 atomic_inc(&universe->failed_del);
 }
 
 #ifdef DEBUG
 if(success){
 DEBUG_PRINT("--Deleted: %ld\n", val);
 }else{
 DEBUG_PRINT("--Failed Deleting: %ld, reason %d\n", val, success);}
 #endif
 
 return success;
 }
 */
