/*
 * File:
 *   test.c
 * Author(s):
	 *   Vincent Gramoli <vincent.gramoli@epfl.ch>
 * Description:
 *   Concurrent accesses to a skip list implementation of an integer set
 *
 * Copyright (c) 2009-2010.
	 *
 * test.c is part of Synchrobench
 * 
 * Synchrobench is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, version 2
 * of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <assert.h>                                                                                                                                                     
#include <getopt.h>                                                                                                                                                     
#include <limits.h>                                                                                                                                                     
#include <pthread.h>                                                                                                                                                    
#include <stdlib.h>                                                                                                                                                     
#include <stdio.h>                                                                                                                                                      
#include <sys/time.h>                                                                                                                                                   
#include <time.h>   
#include <stdint.h>

#include "urcu.h"
#include "citrus.h"

#include "bench.h"

/* Re-entrant version of rand_range(r) */
inline long rand_range_re1(unsigned int *seed, long r) {
	int m = RAND_MAX;
	int d, v = 0;

	/* #ifdef BIAS_RANGE */
	/* 	if(rand_r(seed) < RAND_MAX / 10000) { */
	/* 	  if(last < r || last > r * 10) { */
	/* 	    last = r; */
	/* 	  } */
	/* 	  return last++; */
	/* 	} */
	/* #endif	 */
	do {
		d = (m > r ? r : m);		
		v += 1 + (int)(d * ((double)rand_r(seed)/((double)(m)+1.0)));
		r -= m;
	} while (r > 0);
	return v;
}


inline void *xmalloc(size_t size) {
	void *p = malloc(size);
	if (p == NULL) {
		perror("malloc");
		exit(1);
	}
	return p;
}

typedef intptr_t val_t;

data_t  root;

int main(int argc, char **argv)
{


	unsigned int global_seed;

	int j;
	val_t val = 0;


	int myopt = 0;
	int s, u, n, i, r;       //Various parameters

	i = 1023;           //default initial element count
	r = 5000000;        //default range size
	u = 10;             //default update rate
	s = 0;              //default seed
	n = 1;              //default number of thread


	fprintf(stderr,"\nRCU-based Binary Search Tree\n===============\n\n");
	if(argc < 2)
		fprintf(stderr,"NOTE: No parameters supplied, will continue with defaults\n");

	fprintf(stderr,"Use -h switch for help.\n\n");

	while( EOF != myopt ) {
		myopt = getopt(argc,argv,"r:n:i:u:s:hb:");
		switch( myopt ) {
			case 'r': r = atoi( optarg ); break;
			case 'n': n = atoi( optarg ); break;
			case 'i': i = atoi( optarg ); break;
			case 'u': u = atoi( optarg ); break;
			case 's': s = atoi( optarg ); break;
			case 'h': fprintf(stderr,"Accepted parameters\n");
			fprintf(stderr,"-r <NUM>    : Range size\n");
			fprintf(stderr,"-u <0..100> : Update ratio. 0 = Only search; 100 = Only updates\n");
			fprintf(stderr,"-i <NUM>    : Initial tree size (inital pre-filled element count)\n");
			fprintf(stderr,"-n <NUM>    : Number of threads\n");
			fprintf(stderr,"-s <NUM>    : Random seed. 0 = using time as seed\n");
			fprintf(stderr,"-h          : This help\n\n");
			fprintf(stderr,"Benchmark output format: \n\"0: range, insert ratio, delete ratio, #threads, attempted insert, attempted delete, attempted search, effective insert, effective delete, effective search, time (in msec)\"\n\n");
			exit(0);
		}
	}
	fprintf(stderr,"Parameters:\n");
	fprintf(stderr,"- Range size r:\t\t %d\n", r);
	fprintf(stderr,"- Update rate u:\t %d%% \n", u);
	fprintf(stderr,"- Number of threads n:\t %d\n", n);
	fprintf(stderr,"- Initial tree size i:\t %d\n", i);
	fprintf(stderr,"- Random seed s:\t %d\n", s);

	if (s == 0)
		srand((int)time(0));
	else
		srand(s);

	fprintf(stderr, "Node size: %lu bytes\n", sizeof(node_t));


	root = init(); // initialize the tree

	initURCU(n); // initialize RCU with specific numthreads

	global_seed = rand();

#if !defined(__TEST)

	/* Populate set */
	printf("Adding %d entries to set\n", i);
	j = 0;
	while (j < i) {
		val = rand_range_re1(&global_seed, r);
		if (insert(root, val, 0)) {
			j++;
		}
	}
	start_benchmark(root, r, u, n, 0);

#else

	testpar(root, u, n, 1);
	testseq(root, 1);
    
#endif

	
	return 0;
}
