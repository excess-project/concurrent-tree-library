/*
 * File:
 *   test.c
 * Author(s):
	 *   Tyler Crain <tyler.crain@irisa.fr>
 *   Vincent Gramoli <vincent.gramoli@epfl.ch>
 * Description:
 *   Concurrent accesses to the fast lock-free unbalanced binary search tree
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

#include <getopt.h>
#include <signal.h>
#include <sys/time.h>

#include <math.h>

#include "bst.h"
#include "bst_tk.h"

#include "bench.h"

inline int rand_range_re2(unsigned int *seed, long r) {
    return (rand_r(seed) % r) + 1;
}

int main(int argc, char **argv)
{

	intset_t* set;

#if defined(CLH)
  init_clh_thread(&clh_local_p);
#endif

	ssalloc_init();
	
	int myopt = 0;
	int j = 0;
	int s, u, n, i, r;       //Various parameters

	i = 1023;           //default initial element count
	r = 5000000;        //default range size
	u = 10;             //default update rate
	s = 0;              //default seed
	n = 1;              //default number of thread
	
	
	fprintf(stderr,"\nNon Blocking Binary Search Tree\n===============\n\n");
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
	
	set = set_new();


#if !defined(__TEST)
	
	unsigned int global_seed = rand();

	uint64_t val;

#if GC == 1
	alloc = (ssmem_allocator_t*) malloc(sizeof(ssmem_allocator_t));
	assert(alloc != NULL);
	ssmem_alloc_init_fs_size(alloc, SSMEM_DEFAULT_MEM_SIZE, SSMEM_GC_FREE_SET_SIZE, n);
#endif
	
	/* Populate set */
	printf("Adding %d entries to set\n",i);
	j = 0;
	while (j < i) {
		val = rand_range_re2(&global_seed, r);
		if (bst_tk_insert(set, val, val)) {
			j++;
		}
	}
	
	start_benchmark(set, r, u, n, 0);

#else

	testpar(set, u, n, 1);
	testseq(set, 1);

#endif

	return 0;
}

