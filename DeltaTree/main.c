/*
 main.c

 DeltaTree

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


#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <time.h>
#include <assert.h>

#include "dtree.h"
#include "bench.h"


int main(int argc, char **argv ) {
    int myopt = 0;
    
    int s, u, n, i, t, r, v;       //Various parameters
    
    
    //myname = argv[0];
    
    i = 127;            //default initial element count
    t = 1023;           //default triangle size
    r = 5000000;        //default range size
    u = 10;             //default update rate
    s = 0;              //default seed
    n = 1;              //default number of thread
    
    v=0;                //default valgrind mode (reduce stats)
    
    fprintf(stderr,"\nDeltaTree v0.1\n===============\n\n");
    if(argc < 2)
        fprintf(stderr,"NOTE: No parameters supplied, will continue with defaults\n");
    fprintf(stderr,"Use -h switch for help.\n\n");
    
    while( EOF != myopt ) {
        myopt = getopt(argc,argv,"r:t:n:i:u:s:v:hb:");
        switch( myopt ) {
            case 'r': r = atoi( optarg ); break;
            case 'n': n = atoi( optarg ); break;
            case 't': t = atoi( optarg ); break;
            case 'i': i = atoi( optarg ); break;
            case 'u': u = atoi( optarg ); break;
            case 's': s = atoi( optarg ); break;
            case 'v': v = atof( optarg ); break;
            case 'h': fprintf(stderr,"Accepted parameters\n");
                fprintf(stderr,"-r <NUM>    : Range size\n");
                fprintf(stderr,"-u <0..100> : Update ratio. 0 = Only search; 100 = Only updates\n");
                fprintf(stderr,"-i <NUM>    : Initial tree size (inital pre-filled element count)\n");
                fprintf(stderr,"-t <NUM>    : DeltaNode size\n");
                fprintf(stderr,"-n <NUM>    : Number of threads\n");
                fprintf(stderr,"-s <NUM>    : Random seed. 0 = using time as seed\n");
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
    fprintf(stderr,"- Valgrind mode v:\t %d\n\n", v);
    
    if (s == 0)
		srand((int)time(0));
	else
		srand(s);
    
	greenbst_t* greenbstPtr = greenbst_alloc();
    assert(greenbstPtr);
	
#if !defined(__TEST)
	
    if(i){
        fprintf(stderr,"Now pre-filling %d random elements...\n", i);
        initial_add(greenbstPtr, i, r);
    }
    
    fprintf(stderr, "Finished init a DeltaTree using DeltaNode size %d, with initial %d members\n", greenbstPtr->max_node, i);
    fflush(stderr);
    
    start_benchmark(greenbstPtr, r, u, n, v);
    
#else

	testpar(greenbstPtr, u, n, 1);
	testseq(greenbstPtr, 1);
    
#endif

	exit(0);
	
}


