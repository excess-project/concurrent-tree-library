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
#include "locks.h"
#include "bench.h"

int main(int argc, char **argv ) {
    char myopt = 0;
    struct global *universe = 0;
    
    float  d;
    int s, u, n, i, t, r, v;       //Various parameters
    
    
    //myname = argv[0];
    
    i = 127;           //default initial element count
    t = 1023;            //default triangle size
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
    
    
    init_global(universe);
    
    fprintf(stderr,"Finished building initial DeltaTree\n");
    fprintf(stderr, "The node size is: %ld bytes\n", sizeof(struct node));
    
    create_map(universe->ref, universe->max_node); //Now lets make the map
    
    if(i){
        fprintf(stderr,"Now pre-filling %d random elements...\n", i);
        initial_add(universe, i, r);
    }
    
    fprintf(stderr, "Finished init a DeltaTree using DeltaNode size %d, with initial %d members\n", universe->max_node, i);
    fflush(stderr);
    
    start_benchmark(universe, r, u, n, v);
    exit(0);
}

