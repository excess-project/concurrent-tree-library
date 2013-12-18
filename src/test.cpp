#include <vector>
#include <string>
#include <iostream>
#include <random>
#include <functional>
#include <thread>
#include <algorithm>

#include <sys/time.h>


#ifdef __USEPCM

#include "../intelpcm/cpucounters.h"
using namespace std;

#endif


#include "test.hpp"
#include "HazardManager.hpp" //To manipulate thread_num
#include "tree_type_traits.hpp"

//Include all the trees implementations
#include "skiplist/SkipList.hpp"
#include "nbbst/NBBST.hpp"
#include "avltree/AVLTree.hpp"
#include "lfmst/MultiwaySearchTree.hpp"
#include "cbtree/CBTree.hpp"

//Number of nodes inserted in single-threaded mode
#define ST_N 100000

//Number of nodes inserted in multi-threaded mode (for up to 32 threads)
#define MT_N 100000         //Warning: This number is inserted for each thread

//Utility macros to print debug messages during the tests
#define DEBUG_ENABLED true
#define DEBUG(message) if(DEBUG_ENABLED) std::cout << message << std::endl;

/*!
 * Launch a single threaded test on the given structure. 
 * \param T The type of the structure.
 * \param name The name of the structure being tested. 
 */
template<typename T>
void testST(const std::string& name){
    std::cout << "Test single-threaded (with " << ST_N << " elements) " << name << std::endl;

    thread_num = 0;
    
    T tree;
    
    std::mt19937_64 engine(time(NULL));

    //Note: The max() value cannot be handled by all data structure
    std::uniform_int_distribution<int> distribution(0, std::numeric_limits<int>::max() - 1);
    auto generator = std::bind(distribution, engine);

    DEBUG("Remove numbers in the empty tree");

    for(unsigned int i = 0; i < ST_N; ++i){
        auto number = generator();

        assert(!tree.contains(number));
        assert(!tree.remove(number));
    }
    
    DEBUG("Insert sequential numbers");

    unsigned int sequential_nodes = ST_N;

    if(!is_balanced<T>()){
        sequential_nodes /= 25;
    }

    for(unsigned int i = 0; i < sequential_nodes; ++i){
        assert(!tree.contains(i));
        assert(tree.add(i));
        assert(tree.contains(i));
    }
    
    DEBUG("Remove all the sequential numbers");
    
    for(unsigned int i = 0; i < sequential_nodes; ++i){
        assert(tree.contains(i));
        assert(tree.remove(i));     
        assert(!tree.contains(i));
    }
    
    DEBUG("Verify that the tree is empty");
    
    for(unsigned int i = 0; i < sequential_nodes; ++i){
        assert(!tree.contains(i));
    }

    std::vector<int> rand;
    
    DEBUG("Insert N random numbers in the tree");

    for(unsigned int i = 0; i < ST_N; ++i){
        int number = generator();

        if(tree.contains(number)){
            assert(!tree.add(number));     
            assert(tree.contains(number));
        } else {
            assert(tree.add(number));     
            assert(tree.contains(number));

            rand.push_back(number);
        }
    }
    
    DEBUG("Remove numbers not present in the tree");
    
    for(unsigned int i = 0; i < ST_N; ++i){
        int number = generator();

        if(!tree.contains(number)){
            assert(!tree.remove(number));
            assert(!tree.contains(number));
        }
    }
    
    DEBUG("Remove all the numbers in random order");

    random_shuffle(rand.begin(), rand.end());
    for(int number : rand){
        assert(tree.contains(number));
        assert(tree.remove(number));
    }
    
    DEBUG("Remove numbers in the empty tree");
    
    for(unsigned int i = 0; i < ST_N; ++i){
        auto number = generator();

        assert(!tree.contains(number));
        assert(!tree.remove(number));
    }

    std::cout << "Test passed successfully" << std::endl;
}

/*!
 * Launch the multithreaded tests on the given tree type. 
 * \param T The type of tree to test.
 * \param Threads The number of threads. 
 */
template<typename T, unsigned int Threads>
void testMT(){
    T tree;

    DEBUG("Insert and remove sequential numbers from the tree")

    int sequential_nodes = MT_N;

    if(!is_balanced<T>()){
        sequential_nodes /= 25;
    }

    std::vector<std::thread> pool;
    for(unsigned int i = 0; i < Threads; ++i){
        pool.push_back(std::thread([sequential_nodes, &tree, i](){
            thread_num = i;

            //Insert sequential numbers
            for(unsigned int j = i * sequential_nodes; j < (i + 1) * sequential_nodes; ++j){
                assert(!tree.contains(j));
                assert(tree.add(j));
                assert(tree.contains(j));
            }

            //Remove all the sequential numbers
            for(unsigned int j = i * sequential_nodes; j < (i + 1) * sequential_nodes; ++j){
                assert(tree.contains(j));
                assert(tree.remove(j));   
                assert(!tree.contains(j));
            }
        }));
    }

    for_each(pool.begin(), pool.end(), [](std::thread& t){t.join();});
    pool.clear();
    
    DEBUG("Verify that all the numbers have been removed correctly")
    
    for(unsigned int i = 0; i < Threads; ++i){
        pool.push_back(std::thread([sequential_nodes, &tree, i](){
            thread_num = i;

            //Verify that every numbers has been removed correctly
            for(unsigned int j = 0; j < Threads * sequential_nodes; ++j){
                assert(!tree.contains(j));
            }
        }));
    }

    for_each(pool.begin(), pool.end(), [](std::thread& t){t.join();});
    pool.clear();

    std::vector<unsigned int> fixed_points;
    
    std::mt19937_64 fixed_engine(time(NULL));
    std::uniform_int_distribution<int> fixed_distribution(0, std::numeric_limits<int>::max() - 1);
    
    DEBUG("Compute the fixed points")

    while(fixed_points.size() < Threads){
        auto value = fixed_distribution(fixed_engine);
        
        if(std::find(fixed_points.begin(), fixed_points.end(), value) == fixed_points.end()){
            fixed_points.push_back(value);
            
            assert(tree.add(value));
        }
    }
    
    DEBUG("Make some operations by ensuring that the fixed points are not modified")

    for(unsigned int i = 0; i < Threads; ++i){
        pool.push_back(std::thread([&tree, &fixed_points, i](){
            thread_num = i;

            std::vector<int> rand;
            
            std::mt19937_64 engine(time(0) + i);

            //Note: The max() value cannot be handled by all data structure
            std::uniform_int_distribution<int> distribution(0, std::numeric_limits<int>::max() - 1);
            auto generator = std::bind(distribution, engine);

            std::uniform_int_distribution<int> operationDistribution(0, 99);
            auto operationGenerator = std::bind(operationDistribution, engine);

            for(int n = 0; n < 10000; ++n){
                auto value = generator();

                if(operationGenerator() < 33){
                    if(std::find(fixed_points.begin(), fixed_points.end(), value) == fixed_points.end()){
                        tree.remove(value);
                    }
                } else {
                    tree.add(value);

                    if(std::find(fixed_points.begin(), fixed_points.end(), value) == fixed_points.end()){
                        rand.push_back(value);
                    }
                }

                assert(tree.contains(fixed_points[i]));
            }

            for(auto& value : rand){
                tree.remove(value);
            }
        }));
    }

    for_each(pool.begin(), pool.end(), [](std::thread& t){t.join();});

    for_each(fixed_points.begin(), fixed_points.end(), [&tree](int value){tree.remove(value);});
    
    std::cout << "Test with " << Threads << " threads passed succesfully" << std::endl;
}

/*!
 * Launch all the tests on the given type.
 * \param type The type of the tree. 
 * \param the The name of the tree. 
 */
#define TEST(type, name) \
    std::cout << "Test with 1 threads" << std::endl;\
    testST<type<int, 1>>(name);\
    std::cout << "Test multi-threaded (with " << MT_N << " elements) " << name << std::endl;\
    testMT<type<int, 2>, 2>();\
    testMT<type<int, 3>, 3>();\
    testMT<type<int, 4>, 4>();\
    testMT<type<int, 6>, 6>();\
    testMT<type<int, 8>, 8>();\
    testMT<type<int, 12>, 12>();\
    testMT<type<int, 16>, 16>();\
    testMT<type<int, 32>, 32>();

/*!
 * Test all the different versions.
 */
void test(){
    std::cout << "Tests the different versions" << std::endl;

    //TEST(skiplist::SkipList, "SkipList")
    TEST(nbbst::NBBST, "Non-Blocking Binary Search Tree")
    TEST(avltree::AVLTree, "Optimistic AVL Tree")
    TEST(lfmst::MultiwaySearchTree, "Lock Free Multiway Search Tree");
    TEST(cbtree::CBTree, "Counter Based Tree");
}

#define MAXITER     100000000


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

/* Struct for data input/output per-thread */
struct arg_bench {
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
};

template<typename T, unsigned int Threads>
int benchmark(unsigned int threads, int size, float ins, float del, int initial){
    long *inputs;
    int *ops;
    
    unsigned int i, k;
    
    struct arg_bench *args, *arg;
    
    struct arg_bench result;

    args = static_cast<arg_bench*>(calloc(threads, sizeof(struct arg_bench)));
    
    inputs = static_cast<long*>(calloc(size, sizeof(long)));
    ops = static_cast<int*>(calloc(size, sizeof(int)));

    prepare_randintp(ins, del);
    
    
    T tree;
    
    /* Fill in value based on initial number */
    if(initial > 0){
        std::cout << "Filling tree with initial " << initial << " members..." << std::flush;
        int ix = 0, jx = 0;
        while(ix < initial){
            jx = (rand() % size) + 1;
            if(tree.add(jx))
                ix++;
        }
        std::cout << "Done" << std::endl << std::endl;
    }
    
    
    
    for(i = 0; i< threads; i++){
        arg = &args[i];
        arg->size = size;
        
        arg->update = ins + del;
        arg->seed = rand();
        arg->seed2 = rand();
        arg->pool = static_cast<unsigned*>(calloc(MAX_POOL, sizeof(unsigned)));
        
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
    
    std::vector<std::thread> pool;
    for(i = 0; i < threads; ++i){
        pool.push_back(std::thread([&args, &tree, i](){
            
            thread_num = i;
            
            //std::cout << "Done: " << i << " thread" << std::endl;
            
            static __thread unsigned cont = 0;
            static __thread long counter[3]={0}, success[3]={0};
            static __thread int val = 0;
            static __thread int opsx, ret = 0;
            static __thread struct timeval start, end;
            
            gettimeofday(&start, NULL);
            
            while(cont < args[i].max_iter){
                
                //--For a completely random values (original)
                opsx = args[i].pool[rand_range_re(&args[i].seed, MAX_POOL) - 1];
                val = rand_range_re(&args[i].seed2, args[i].size);
                
                switch (opsx){
                    case 1: ret = tree.add(val); break;
                    case 2: ret = tree.remove(val); break;
                    case 3: ret = tree.contains(val); break;
                    default: exit(0); break;
                }
                cont++;
                counter[opsx-1]++;
                if(ret)
                    success[opsx-1]++;
                
                
            }
            
            gettimeofday(&end, NULL);
     
             args[i].counter_ins = counter[0];
             args[i].counter_del = counter[1];
             args[i].counter_search = counter[2];
             
             args[i].counter_ins_s = success[0];
             args[i].counter_del_s = success[1];
             args[i].counter_search_s = success[2];
             
             args[i].timer =(end.tv_sec * 1000 + end.tv_usec / 1000) - (start.tv_sec * 1000 + start.tv_usec / 1000);
     
        }));
    }
    
#ifdef __USEPCM
    
    PCM * m = PCM::getInstance();
    
    if (m->program() != PCM::Success) return 0;
    
    SystemCounterState before_sstate = getSystemCounterState();
    
#endif
    
    fprintf(stderr, "\nStarting benchmark..");
    
    fprintf(stderr, "\n0: %d, %0.2f, %0.2f, %d, ", size, ins, del, threads);
    

    for_each(pool.begin(), pool.end(), [](std::thread& t){t.join();});

#ifdef __USEPCM
    
    SystemCounterState after_sstate = getSystemCounterState();
    
    cout << "Instructions per clock: " << getIPC(before_sstate,after_sstate) << endl
    << "L3 cache hit ratio: " << getL3CacheHitRatio(before_sstate,after_sstate) << endl
    << "Bytes read: " << getBytesReadFromMC(before_sstate,after_sstate) << endl
    << "Power used: " << getConsumedJoules(before_sstate,after_sstate) <<" joules"<< endl
    << std::endl;
    
#endif

    
    pool.clear();
    
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
        result.timer = result.timer + arg->timer;
        
    }
    
    fprintf(stderr, " %ld, %ld, %ld,", result.counter_ins, result.counter_del, result.counter_search);
    fprintf(stderr, " %ld, %ld, %ld, %ld\n", result.counter_ins_s, result.counter_del_s, result.counter_search_s, result.timer);
    
    free(inputs);
    free(ops);
    free(args);
    
    return 0;
}

void start_benchmark(int initial, int key_size, int updaterate, int num_thread, int treetype){
    
    float update = (float)updaterate/2;
    
    if(treetype == 0){
        //std::cout << "Non-Blocking Binary Search Tree" << std::endl;
        switch(num_thread){
            case 1: benchmark<nbbst::NBBST<int, 1>, 1>(1, key_size, update, update, initial); break;
            case 2: benchmark<nbbst::NBBST<int, 2>, 2>(2, key_size, update, update, initial); break;
            case 3: benchmark<nbbst::NBBST<int, 3>, 3>(3, key_size, update, update, initial); break;
            case 4: benchmark<nbbst::NBBST<int, 4>, 4>(4, key_size, update, update, initial); break;
            case 5: benchmark<nbbst::NBBST<int, 5>, 5>(5, key_size, update, update, initial); break;
            case 6: benchmark<nbbst::NBBST<int, 6>, 6>(6, key_size, update, update, initial); break;
            case 7: benchmark<nbbst::NBBST<int, 7>, 7>(7, key_size, update, update, initial); break;
            case 8: benchmark<nbbst::NBBST<int, 8>, 8>(8, key_size, update, update, initial); break;
            case 9: benchmark<nbbst::NBBST<int, 9>, 9>(9, key_size, update, update, initial); break;
            case 10: benchmark<nbbst::NBBST<int, 10>, 10>(10, key_size, update, update, initial); break;
            case 11: benchmark<nbbst::NBBST<int, 11>, 11>(11, key_size, update, update, initial); break;
            case 12: benchmark<nbbst::NBBST<int, 12>, 12>(12, key_size, update, update, initial); break;
            case 13: benchmark<nbbst::NBBST<int, 13>, 13>(13, key_size, update, update, initial); break;
            case 14: benchmark<nbbst::NBBST<int, 14>, 14>(14, key_size, update, update, initial); break;
            case 15: benchmark<nbbst::NBBST<int, 15>, 15>(15, key_size, update, update, initial); break;
            case 16: benchmark<nbbst::NBBST<int, 16>, 16>(16, key_size, update, update, initial); break;
            default: break;
        }
    }else if(treetype == 1){
        //std::cout << "Optimistic AVL Tree" << std::endl;
        switch(num_thread){
            case 1: benchmark<avltree::AVLTree<int, 1>, 1>(1, key_size, update, update, initial); break;
            case 2: benchmark<avltree::AVLTree<int, 2>, 2>(2, key_size, update, update, initial); break;
            case 3: benchmark<avltree::AVLTree<int, 3>, 3>(3, key_size, update, update, initial); break;
            case 4: benchmark<avltree::AVLTree<int, 4>, 4>(4, key_size, update, update, initial); break;
            case 5: benchmark<avltree::AVLTree<int, 5>, 5>(5, key_size, update, update, initial); break;
            case 6: benchmark<avltree::AVLTree<int, 6>, 6>(6, key_size, update, update, initial); break;
            case 7: benchmark<avltree::AVLTree<int, 7>, 7>(7, key_size, update, update, initial); break;
            case 8: benchmark<avltree::AVLTree<int, 8>, 8>(8, key_size, update, update, initial); break;
            case 9: benchmark<avltree::AVLTree<int, 9>, 9>(9, key_size, update, update, initial); break;
            case 10: benchmark<avltree::AVLTree<int, 10>, 10>(10, key_size, update, update, initial); break;
            case 11: benchmark<avltree::AVLTree<int, 11>, 11>(11, key_size, update, update, initial); break;
            case 12: benchmark<avltree::AVLTree<int, 12>, 12>(12, key_size, update, update, initial); break;
            case 13: benchmark<avltree::AVLTree<int, 13>, 13>(13, key_size, update, update, initial); break;
            case 14: benchmark<avltree::AVLTree<int, 14>, 14>(14, key_size, update, update, initial); break;
            case 15: benchmark<avltree::AVLTree<int, 15>, 15>(15, key_size, update, update, initial); break;
            case 16: benchmark<avltree::AVLTree<int, 16>, 16>(16, key_size, update, update, initial); break;
            default: break;
        }
    }else if(treetype == 2){
        //std::cout << "Lock Free Multiway Search Tree" << std::endl;
        switch(num_thread){
            case 1: benchmark<lfmst::MultiwaySearchTree<int, 1>, 1>(1, key_size, update, update, initial); break;
            case 2: benchmark<lfmst::MultiwaySearchTree<int, 2>, 2>(2, key_size, update, update, initial); break;
            case 3: benchmark<lfmst::MultiwaySearchTree<int, 3>, 3>(3, key_size, update, update, initial); break;
            case 4: benchmark<lfmst::MultiwaySearchTree<int, 4>, 4>(4, key_size, update, update, initial); break;
            case 5: benchmark<lfmst::MultiwaySearchTree<int, 5>, 5>(5, key_size, update, update, initial); break;
            case 6: benchmark<lfmst::MultiwaySearchTree<int, 6>, 6>(6, key_size, update, update, initial); break;
            case 7: benchmark<lfmst::MultiwaySearchTree<int, 7>, 7>(7, key_size, update, update, initial); break;
            case 8: benchmark<lfmst::MultiwaySearchTree<int, 8>, 8>(8, key_size, update, update, initial); break;
            case 9: benchmark<lfmst::MultiwaySearchTree<int, 9>, 9>(9, key_size, update, update, initial); break;
            case 10: benchmark<lfmst::MultiwaySearchTree<int, 10>, 10>(10, key_size, update, update, initial); break;
            case 11: benchmark<lfmst::MultiwaySearchTree<int, 11>, 11>(11, key_size, update, update, initial); break;
            case 12: benchmark<lfmst::MultiwaySearchTree<int, 12>, 12>(12, key_size, update, update, initial); break;
            case 13: benchmark<lfmst::MultiwaySearchTree<int, 13>, 13>(13, key_size, update, update, initial); break;
            case 14: benchmark<lfmst::MultiwaySearchTree<int, 14>, 14>(14, key_size, update, update, initial); break;
            case 15: benchmark<lfmst::MultiwaySearchTree<int, 15>, 15>(15, key_size, update, update, initial); break;
            case 16: benchmark<lfmst::MultiwaySearchTree<int, 16>, 16>(16, key_size, update, update, initial); break;
            default: break;
        }
    }else if(treetype == 3){
        //std::cout << "Counter Based Tree" << std::endl;
        switch(num_thread){
            case 1: benchmark<cbtree::CBTree<int, 1>, 1>(1, key_size, update, update, initial); break;
            case 2: benchmark<cbtree::CBTree<int, 2>, 2>(2, key_size, update, update, initial); break;
            case 3: benchmark<cbtree::CBTree<int, 3>, 3>(3, key_size, update, update, initial); break;
            case 4: benchmark<cbtree::CBTree<int, 4>, 4>(4, key_size, update, update, initial); break;
            case 5: benchmark<cbtree::CBTree<int, 5>, 5>(5, key_size, update, update, initial); break;
            case 6: benchmark<cbtree::CBTree<int, 6>, 6>(6, key_size, update, update, initial); break;
            case 7: benchmark<cbtree::CBTree<int, 7>, 7>(7, key_size, update, update, initial); break;
            case 8: benchmark<cbtree::CBTree<int, 8>, 8>(8, key_size, update, update, initial); break;
            case 9: benchmark<cbtree::CBTree<int, 9>, 9>(9, key_size, update, update, initial); break;
            case 10: benchmark<cbtree::CBTree<int, 10>, 10>(10, key_size, update, update, initial); break;
            case 11: benchmark<cbtree::CBTree<int, 11>, 11>(11, key_size, update, update, initial); break;
            case 12: benchmark<cbtree::CBTree<int, 12>, 12>(12, key_size, update, update, initial); break;
            case 13: benchmark<cbtree::CBTree<int, 13>, 13>(13, key_size, update, update, initial); break;
            case 14: benchmark<cbtree::CBTree<int, 14>, 14>(14, key_size, update, update, initial); break;
            case 15: benchmark<cbtree::CBTree<int, 15>, 15>(15, key_size, update, update, initial); break;
            case 16: benchmark<cbtree::CBTree<int, 16>, 16>(16, key_size, update, update, initial); break;
            default: break;
        }
    }
}

