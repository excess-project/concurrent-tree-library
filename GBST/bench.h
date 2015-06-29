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

#ifndef bench_h
#define bench_h

#define __THREAD_PINNING 1

#ifdef __USEPCM
#include "../benchcounters.h"
#endif

#include "common.h"

void *t_add (struct global* universe, int range);

pthread_barrier_t bench_barrier;

/* RANDOM GENERATOR */


// RANGE: (1 - r)
inline int rand_range_re(unsigned int *seed, long r) {
    return (rand_r(seed) % r) + 1;
}


/* simple function for generating random integer for probability, only works on value of integer 1-100% */

#define MAX_POOL 1000

int p_pool[MAX_POOL];

void prepare_randintp(float ins, float del) {
    
    int i,j=0;
    
    //Put insert
    for(i = 0;i < ins * MAX_POOL/100; i++){
        p_pool[j++]=1;
    }
    //Put delete
    for(i = 0; i< del* MAX_POOL/100; i++){
        p_pool[j++]=2;
    }
    //Put search
    for(i = j; i < MAX_POOL; i++){
        p_pool[j++]=3;
    }
    
    /*
    fprintf(stderr,"\n");
    for (i = 0; i < MAX_POOL; i++)
        fprintf(stderr, "%d, ", p_pool[i]);
    fprintf(stderr,"\n");
    */
}

/* SIGNALS */
/* This flag controls termination of the main loop. */
volatile sig_atomic_t keep_going = 1;

/* Struct for data input/output per-thread */
struct arg_bench {
    unsigned rank;
    int size;
    unsigned seed;
    unsigned seed2;
    unsigned update;
    unsigned *pool;
    unsigned long max_iter;
    long counter_ins;
    long counter_del;
    long counter_search;
    long counter_ins_s;
    long counter_del_s;
    long counter_search_s;
    long timer;
    long *inputs;
    int *ops;
    struct global* universe;
};


/* The signal handler just clears the flag and re-enables itself. */
void catch_alarm (int sig)
{
    keep_going = 0;
    signal (sig, catch_alarm);
}


void* do_bench (void* arguments)
{
    long counter[3]={0}, success[3]={0};
    int val = 0;// last = 0;
    unsigned *pool;
    int ops, ret = 0;
    struct global* universe;
    int b_size;
    long cont = 0;
    long max_iter = 0;
    
    struct timeval start, end;
    struct arg_bench *args = arguments;
    
    
    max_iter = args->max_iter;
    universe = args->universe;
    b_size = args->size;
    pool = args->pool;
    
    //fprintf(stderr, "seed1:%d, seed2:%d, iter: %ld\n", args->seed, args->seed2, max_iter);

#ifndef __APPLE__
#if (__THREAD_PINNING == 1)
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(args->rank, &cpuset);
    
    pthread_t current_thread = pthread_self();
    
    fprintf(stdout, "Pinning to core %d... %s\n", args->rank, pthread_setaffinity_np(current_thread, sizeof(cpu_set_t), &cpuset)==0?"Success":"Failed");
#endif
#endif
    
    pthread_barrier_wait(&bench_barrier);

    gettimeofday(&start, NULL);
    
    /* Check the flag once in a while to see when to quit. */
    while(cont < max_iter){
        
        /*---------------------------*/
        
        //--For a completely random values (original)
        ops = pool[rand_range_re(&args->seed, MAX_POOL) - 1];
        val = rand_range_re(&args->seed2, b_size);
        
        DEBUG_PRINT("ops:%d, val:%d\n", ops, val);
        
        switch (ops){
            case 1: ret = insertNode(universe, val); break;
            case 2: ret = deleteNode(universe, val); break;
            case 3: ret = searchNode(universe, val); break;
            default: exit(0); break;
        }
        cont++;
        counter[ops-1]++;
        if(ret)
            success[ops-1]++;
        
    }
    
    gettimeofday(&end, NULL);

    args->counter_ins = counter[0];
    args->counter_del = counter[1];
    args->counter_search = counter[2];
    
    args->counter_ins_s = success[0];
    args->counter_del_s = success[1];
    args->counter_search_s = success[2];
    
    args->timer =(end.tv_sec * 1000 + end.tv_usec / 1000) - (start.tv_sec * 1000 + start.tv_usec / 1000);
	
    
    DEBUG_PRINT("\ncount:%ld, ins:%ld, del:%ld, search:%ld", cont, args->counter_ins, args->counter_del, args->counter_search);
    
    
    pthread_exit((void*) arguments);
}


int benchmark(struct global* universe, int threads, int size, float ins, float del){
    pthread_t *pid;
    long *inputs;
    int *ops;
    
    int i, k;
    
    struct arg_bench *args, *arg;
    
    struct arg_bench result;

    args = calloc(threads, sizeof(struct arg_bench));
    
    inputs = calloc(size, sizeof(long));
    ops = calloc(size, sizeof(int));

    prepare_randintp(ins, del);
    
    long ncores = sysconf( _SC_NPROCESSORS_ONLN );
    int midcores = (int)ncores/2;
    
    for(i = 0; i< threads; i++){
        arg = &args[i];
        if(threads > midcores && threads < ncores){
            if(i >= (threads/2))
                arg->rank = i - (threads/2) + midcores;
            else
                arg->rank = i;
        }else
            arg->rank = i;
        arg->size = size;
        arg->universe = universe;
        
        arg->update = ins + del;
        arg->seed = rand();
        arg->seed2 = rand();
        arg->pool = calloc(MAX_POOL, sizeof(unsigned));
        
        for (k = 0; k < MAX_POOL; k++)
            arg->pool[k] = p_pool[k];
        
        arg->counter_ins = 0;
        arg->counter_del = 0;
        arg->counter_search = 0;
    
        arg->counter_ins_s = 0;
        arg->counter_del_s = 0;
        arg->counter_search_s = 0;
        
        arg->timer = 0;
        
        arg->inputs = inputs;
        arg->ops = ops;
        
        arg->max_iter = ceil(MAXITER / threads);
        
    }
    
    
   /*
    for(j = 0; j < size; j++)
        inputs[j] = (rand() % size) + 1;
    
    for(j = 0; j < size; j++)
        ops[j] =  p_pool[(rand() % 100)];
    */
    pid = calloc(threads, sizeof(pthread_t));
    
    //keep_going = 1;             //reset the flag
    
    /* Establish a handler for SIGALRM signals. */
    //signal (SIGALRM, catch_alarm);
    
    /* Set an alarm to go off in a little while. */
    //alarm (TIME_PER_BENCH);
    
    pthread_barrier_init(&bench_barrier,NULL,threads);
    
    struct timeval _ts;
    gettimeofday(&_ts, NULL);
    fprintf(stderr, "\n#TS: %ld, %d\n", _ts.tv_sec, _ts.tv_usec);
    
#ifdef __USEPCM
	pcm_bench_start();
#endif
    
    fprintf(stderr, "\nStarting benchmark...");
    
    fprintf(stderr, "\n0: %d, %0.2f, %0.2f, %d, ", size, ins, del, threads);
    
    
    for (i = 0; i<threads; i++)
        pthread_create (&pid[i], NULL, &do_bench, &args[i]);
    
    for (i = 0; i<threads; i++)
        pthread_join (pid[i], NULL);
    
    
#ifdef __USEPCM
	pcm_bench_end();
	pcm_bench_print();
#endif
    
    result.counter_del = 0;
    result.counter_del_s = 0;
    result.counter_ins = 0;
    result.counter_ins_s = 0;
    result.counter_search = 0;
    result.counter_search_s = 0;
    result.timer = 0;
    
    for(i = 0; i< threads; i++){
        arg = &args[i];
        
        result.counter_del = result.counter_del + arg->counter_del;
        result.counter_del_s = result.counter_del_s + arg->counter_del_s;
        result.counter_ins = result.counter_ins + arg->counter_ins;
        result.counter_ins_s = result.counter_ins_s + arg->counter_ins_s;
        result.counter_search = result.counter_search + arg->counter_search;
        result.counter_search_s = result.counter_search_s + arg->counter_search_s;
        //result.timer = result.timer + arg->timer;
        if(arg->timer > result.timer)
            result.timer = arg->timer;
        
        /*
        fprintf(stderr, "\n(%d): %ld,%ld,%ld,%ld", i, arg->counter_ins, arg->counter_del, arg->counter_search, arg->timer);
        fprintf(stderr, "\n(%d): %ld,%ld,%ld,%ld", i, arg->counter_ins_s, arg->counter_del_s, arg->counter_search_s, arg->timer);
        */
    }
    
    fprintf(stderr, " %ld, %ld, %ld,", result.counter_ins, result.counter_del, result.counter_search);
    fprintf(stderr, " %ld, %ld, %ld, %ld\n", result.counter_ins_s, result.counter_del_s, result.counter_search_s, result.timer);
    
    free(pid);
    free(inputs);
    free(ops);
    free(args);
    
    return 0;
}

void initial_add (struct global* universe, int num, int range) {
    int i = 0, j = 0;
    while(i < num){
        j = (rand()%range) + 1;
        if(insertNode(universe, j))
            i++;
    }
}

int rec_update_counters(struct node* tree, struct global* universe){
    
    unsigned currID;
    long travcount = 0; long allcount = 0;
    
    if(!tree) return 0;
    
    if(tree->tid > 0){                            //We have finished with the current tree, now move on
        currID = (unsigned)tree->tid;             //Get the current tree ID
        
        travNode(tree, EMPTY, 0, nop, &travcount, &allcount);
        if(travcount > universe->max_node)
            universe->aux.row[currID-1].count_node = (int)(universe->max_node+1)/2;
        else
            universe->aux.row[currID-1].count_node = (int)travcount;
        
        //fprintf(stderr,"Tree ID %d, count is: %d\n", currID, universe->aux.row[currID-1].count_node);
    }
    rec_update_counters(tree->left, universe);
    rec_update_counters(tree->right, universe);
    
    
    return 0;
}

void initial_add_balanced (struct global* universe, int num, int range, int validate) {
    int count = num;
    int shift = num;
    int lastval = 0;
    int *buffer, *initial;
    
    int mindepth, maxdepth;
    
    int ii, jj, value;
    
    
    mindepth = maxdepth = (int)ceil(log(num)/log(2)+1);
    
    //fprintf(stderr, "maxdepth, mindepth, space: %d, %d, %d\n", maxdepth, mindepth, universe->max_depth);
    
    if(universe->max_depth < maxdepth)
        build_balanced_height(universe, universe->aux.row[0].root, maxdepth, 0);
    
    //Make index of range...
    initial = malloc(sizeof(int)*range);
    for(ii = 0; ii < range; ii++)
        initial[ii] = ii + 1;
    
    //...shuffle it...
    for(ii=range-1; ii > 1; ii--){
        jj = rand()%range;
        value = initial[jj];
        initial[jj] = initial[ii];
        initial [ii] = value;
    }
    
    //...and finally fill the buffer
    buffer = malloc(sizeof(int)* num);
    
    for(ii = 0; ii < count;ii++){
        buffer[ii] = initial[ii];
    }
    
    sort_all(buffer, num);
    
    //print_int_array(buffer, num);
    
    fill_val_imp(universe->aux.row[0].root, buffer, &count, &shift, &lastval, 0, mindepth, maxdepth, count);
    
    count = num;
    
    if(validate)
        for(ii=0 ; ii<count ; ii++){
            if(!searchNode(universe, buffer[ii] )){
                fprintf(stderr, "Error during building and prefilling a balanced tree!");
                exit(1);
            }
        }
    free(initial);
    free(buffer);
    
    /* Update the counters */
    universe->nodecnt = num;
    universe->maxcnt = num;
     
    //Recursively update counters on each DeltaNode
    rec_update_counters(universe->aux.row[0].root, universe);
}


void start_benchmark(struct global *universe, int key_size, int updaterate, int num_thread, int v){
    
    float update = (float)updaterate/2;
    
    benchmark(universe, num_thread, key_size, update, update);
    
    if(!v){
        //Calculate number of triangle and min/max height
        printMaxMin(universe);
    
        //Calculate number of node
        
        //travcount = 0; allcount = 0;
        //travNode(universe->root, EMPTY, 0, nop); fprintf(stderr,"=> %d, %d\n", travcount, allcount);
        
        //Print op stats
        printStat(universe);
        
    }
}

/* Various Bench Functions */


void *t_add (struct global* universe, int range) {
    int i = 0, j = 0, k = 0;
    for(i=0;i<1000000;i++) {
        j = (rand()%range) + 1;
        if(insertNode(universe, j)){
            //fprintf(stderr, "%d ", j);
            k++;
        }
    }
    //fprintf(stderr, "=>%d successful addition.\n\n", k);
    return NULL;
}

void *t_del (struct global* universe, int range) {
    int i, j, k = 0;
    for(i=0;i<1000000;i++) {
        j = (rand()%range) + 1;
        if(deleteNode(universe, j)){
            fprintf(stderr, "%d ", j);
            k++;
        }
    }
    fprintf(stderr, "=>%d successful deletion.\n\n", k);
    return NULL;
}

void *t_find (struct global* universe, int range) {
    int i;
    for(i=0;i<1000000;i++) searchNode(universe, (rand()%range) + 1);
    return NULL;
}

// Additional tests (for correctness)

void *testall(struct global *universe){

int ii = 0, result = 0;
int temp;
struct node * p;
    
unsigned seed, seed_seed;

seed_seed = rand();
    
seed = seed_seed;
    
fprintf(stderr, "Seed: %u\n", seed);

for(ii=100000; ii >= 1; ii--){
    //temp = ii + 1;
    //fprintf(stderr, "-----$ %d insert\n", ii);
    
    temp = (rand_r(&seed)) + 1;
    insertNode(universe, temp);
  
    //if(!result)
    //    fprintf(stderr, "-----$ %ld failed insert\n", temp);
    
    /*
    p = smart_it_search(universe->aux.row[0].root, temp);
    
    if (p->value == temp)
        fprintf(stderr, "-----$ %ld Inserted and found\n", p->value);
    else
        fprintf(stderr, "-----$ Error inserting: %ld, found %ld (%d)\n", temp, p->value, ii);*/
}
    
sleep(10);          //Wait for completion

seed = seed_seed;

for(ii=100000; ii >= 1; ii--){
    //temp = ii + 1;
    temp = (rand_r(&seed)) + 1;
    p = smart_it_search(universe->aux.row[0].root, temp);
    
    if (p->value == temp )
        fprintf(stderr, "-----$ %d found !\n", p->value);
    else
        fprintf(stderr, "-----$ Error finding: %d, found %d\n", temp, p->value);
}

seed = seed_seed;

for(ii=100000; ii >= 1; ii--){
    //temp = ii + 1;
    temp = (rand_r(&seed)) + 1;
    result = deleteNode(universe, temp);
    p = smart_it_search(universe->aux.row[0].root, temp);
    if (!result || p->value != temp || (p->value == temp && p->mark == 1))
        fprintf(stderr, "-----$ %d found and deleted \n", p->value);
    else
        fprintf(stderr, "-----$ Error deleting: %d, found %d, mark: %d\n", temp, p->value, p->mark);
}
    
    
    traverse_all(universe, 0);
    printMaxMin(universe);
    printStat(universe);
    
    //report_all(universe->aux.row[0].root);
    
    pthread_exit(NULL);

}

#endif
