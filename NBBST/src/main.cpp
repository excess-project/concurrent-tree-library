#include <iostream>
#include <getopt.h>

#include "test.hpp"
#include "bench.hpp"

/*!
 * Launch the test indicated by the arguments.
 */
int main(int argc, const char* argv[]) {

  
    int s, u, n, i, r, t;       //Various parameters
        
    i = 1023;           //default initial element count
    r = 5000000;        //default range size
    u = 10;             //default update rate
    s = 0;              //default seed
    n = 1;              //default number of thread
    
    t = 0;              //default nbbt, lfmst, cbtree mode (reduce stats)
    
    fprintf(stderr,"\n(NOT!) DeltaTree\n===============\n\n");

  	int myopt;
  
 while( EOF != myopt ) {
        myopt = getopt(argc,(char **)argv,"r:t:n:i:u:s:d:h:");
        switch( myopt ) {
    
            case 'r': r = atoi( optarg ); break;
            case 'n': n = atoi( optarg ); break;
            case 'i': i = atoi( optarg ); break;
            case 'u': u = atoi( optarg ); break;
            case 's': s = atoi( optarg ); break;
            case 't': t = atof( optarg ); break;
            case 'h': fprintf(stderr,"Accepted parameters\n");
                fprintf(stderr,"-r <NUM>    : Range size\n");
                fprintf(stderr,"-u <0..100> : Update ratio. 0 = Only search; 100 = Only updates\n");
                fprintf(stderr,"-i <NUM>    : Initial tree size (inital pre-filled element count)\n");
                fprintf(stderr,"-n <NUM>    : Number of threads\n");
                fprintf(stderr,"-s <NUM>    : Random seed. 0 = using time as seed\n");
                fprintf(stderr,"-v <0,1,2,3>: Concurrent tree type. 0 = Non-Blocking Binary Search Tree (default); 1 = Optimistic AVL Tree; 2 = Lock Free Multiway Search Tree; 3 = Counter Based Tree\n");
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
    fprintf(stderr,"- Concurrent tree type t:%d = ", t);
    
    if(t == 0){
        std::cout << "Non-Blocking Binary Search Tree" << std::endl;
    }else if(t == 1){
        std::cout << "Optimistic AVL Tree" << std::endl;
    }else if(t == 2){
        std::cout << "Lock Free Multiway Search Tree" << std::endl;
    }else if(t == 3){
        std::cout << "Counter Based Tree" << std::endl;
    }else{
        std::cout << "ERROR!!" << std::endl;
        return 1;
    }

    std::cout << std::endl;
    
    if (s == 0)
		srand((int)time(0));
	else
		srand(s);

    start_benchmark(i, r, u, n, t);
    
/*

    //By default launch perf test
    if(argc == 1){
        bench();
    } else if(argc == 2) {
        std::string arg = argv[1];

        if(arg == "-perf"){
            bench();
        } else if(arg == "-test"){
            test();
        } else if(arg == "-all"){
            test();
            bench();
        } else {
            std::cout << "Unrecognized option " << arg << std::endl;
        }
    } else {
        std::cout << "Too many arguments" << std::endl;
    }
*/
    return 0;
}
