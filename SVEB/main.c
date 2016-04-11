/*
 main.c

 (global lock-based) STATIC IMPLICIT VEB Tree

 Based on the code from:
 G. S. Brodal, R. Fagerberg, and R. Jacob, “Cache oblivious search trees via binary trees of small height,”
 in Proceedings of the thirteenth annual ACM-SIAM symposium on Discrete algorithms, ser. SODA ’02, 2002, pp. 39–48.
 
 Copyright belongs to the authors.

 */


#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <time.h>
#include <pthread.h>

#include "staticvebtree.h"
#include "bench.h"


int main(int argc, char **argv ) {
    int myopt = 0;

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
                fprintf(stderr,"-t <NUM>    : VEB  size\n");
                fprintf(stderr,"-n <NUM>    : Number of threads\n");
                fprintf(stderr,"-s <NUM>    : Random seed. 0 = using time as seed\n");
                fprintf(stderr,"-h          : This help\n\n");
                fprintf(stderr,"Benchmark output format: \n\"0: range, insert ratio, delete ratio, #threads, attempted insert, attempted delete, attempted search, effective insert, effective delete, effective search, time (in msec)\"\n\n");
                exit(0);
        }
    }
    fprintf(stderr,"Parameters:\n");
    fprintf(stderr,"- Range size r:\t\t %d\n", r);
    fprintf(stderr,"- VEB size t:\t %d\n", t);
    fprintf(stderr,"- Update rate u:\t %d%% \n", u);
    fprintf(stderr,"- Number of threads n:\t %d\n", n);
    fprintf(stderr,"- Initial tree inserts i:\t %d\n", i);
    fprintf(stderr,"- Random seed s:\t %d\n", s);

    if (s == 0)
		srand((int)time(0));
	else
		srand(s);



    init_tree(t);

#if !defined(__TEST)
	
    if(i){
        fprintf(stderr,"Now pre-filling %d random elements...\n", i);
        initial_add(i, r);
    }
    
    fprintf(stderr, "Finished init a DeltaTree using DeltaNode size %d, with initial %d members\n", r, i);
    fflush(stderr);
    
    start_benchmark(NULL, r, u, n, v);
    
#else

	testpar(NULL, u, n, 1);
	testseq(NULL, 1);
    
#endif

	exit(0);
	
}


