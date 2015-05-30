#include <malloc.h>
#include <pthread.h>
#include <unordered_map>

static void* (*old_malloc_hook)(size_t, const void*);
static void (*old_free_hook)(void*, const void*);

static unsigned long allocated = 0;

static bool end = false;
static std::unordered_map<void*, size_t> sizes;

static void* rqmalloc_hook(size_t size, const void* source);
static void rqfree_hook(void* memory, const void* source);

static void* rqmalloc_hook(size_t size, const void* /* source*/){
    void* result;
    
    __malloc_hook = old_malloc_hook;
    __free_hook = old_free_hook;
    
    result = malloc(size);

    old_malloc_hook = __malloc_hook;
    old_free_hook = __free_hook;

    if(!end){
        sizes[result] = size;
        allocated += size;
    }

    __malloc_hook = rqmalloc_hook;
    __free_hook = rqfree_hook;

    return result;
}

static void rqfree_hook(void* memory, const void* /* source */){
    __malloc_hook = old_malloc_hook;
    __free_hook = old_free_hook;
    
    free(memory);

    old_malloc_hook = __malloc_hook;
    old_free_hook = __free_hook;
    
    if(!end){
        allocated -= sizes[memory];
    }

    __malloc_hook = rqmalloc_hook;
    __free_hook = rqfree_hook;
}

void memory_init(){
    old_malloc_hook = __malloc_hook;
    old_free_hook = __free_hook;
    __malloc_hook = rqmalloc_hook;
    __free_hook = rqfree_hook;
}

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <set>

#include "Results.hpp"

//Include all the trees implementations
#include "skiplist/SkipList.hpp"
#include "nbbst/NBBST.hpp"
#include "avltree/AVLTree.hpp"
#include "lfmst/MultiwaySearchTree.hpp"
#include "cbtree/CBTree.hpp"

/*!
 * Launch the memory test on the given Tree. 
 * \param Tree The type of the tree. 
 * \param name The name of the tree. 
 * \param size The number of elements to insert into the tree. 
 * \param results The results to fill. 
 */
template<typename Tree>
void memory(const std::string& name, unsigned int size, Results& results){
    std::vector<int> elements;
    for(unsigned int i = 0; i < size; ++i){
        elements.push_back(i);
    }

    //Use random insertion in order to support non-balanced version
    random_shuffle(elements.begin(), elements.end());

    //For now on, count all the allocations
    allocated = 0;

    Tree* alloc_tree = new Tree();
    Tree& tree = *alloc_tree;

    //Fill the tree
    for(unsigned int i = 0; i < size; ++i){
        tree.add(elements[i]);
    }
    
    unsigned long usage = allocated;
    
    std::cout << name << "-" << size << " is using " << (usage / 1024) << " KB" << std::endl;
    results.add_result(name, (usage / 1024.0));
    
    //Empty the tree
    for(unsigned int i = 0; i < size; ++i){
        tree.remove(i);
    }

    delete alloc_tree;
}

/*!
 * Launch the memory test on the given Tree. The elements are taken in the range [0, INT_MAX - 1].
 * \param Tree The type of the tree. 
 * \param name The name of the tree. 
 * \param size The number of elements to insert into the tree. 
 * \param results The results to fill. 
 */
template<typename Tree>
void memory_high(const std::string& name, unsigned int size, Results& results){
    std::mt19937_64 engine(time(0));

    //Note: The max() value cannot be managed by some structure
    std::uniform_int_distribution<int> valueDistribution(0, std::numeric_limits<int>::max() - 1);
    auto valueGenerator = std::bind(valueDistribution, engine);
    
    std::set<int> elements;
    while(elements.size() < size){
        elements.insert(valueGenerator());
    }

    std::vector<int> vector_elements;
    for(auto i : elements){
        vector_elements.push_back(i);
    }

    //It is necessary to shuffle as the iteration through set is sorted
    random_shuffle(vector_elements.begin(), vector_elements.end());

    //For now on, count all the allocations
    allocated = 0;

    Tree* alloc_tree = new Tree();
    Tree& tree = *alloc_tree;

    for(auto i : vector_elements){
        tree.add(i);
    }

    unsigned long usage = allocated;
    
    std::cout << name << "-" << size << " is using " << (usage / 1024) << " KB" << std::endl;
    results.add_result(name, (usage / 1024.0));
    
    //Empty the tree
    for(auto i : vector_elements){
        tree.remove(i);
    }

    delete alloc_tree;
}

/*!
 * Launch the memory tests depending on the arguments
 */
int main(int argc, const char* argv[]) {
    memory_init();

    std::vector<unsigned int> little_sizes = {1000, 10000, 100000};
    std::vector<unsigned int> big_sizes = {1000000, 10000000};

    thread_num = 0;

    if(argc == 1){
        std::cout << "low or high argument needed" << std::endl;
    } else {
        std::string arg = argv[1];

        if(arg == "low"){
            std::cout << "Test the normal memory consumption of each version" << std::endl;

            Results results;
            results.start("memory-little");
            results.set_max(3);

            for(auto size : little_sizes){
                memory<skiplist::SkipList<int, 32>>("skiplist", size, results);
                memory<nbbst::NBBST<int, 32>>("nbbst", size, results);
                memory<lfmst::MultiwaySearchTree<int, 32>>("lfmst", size, results);
                memory<avltree::AVLTree<int, 32>>("avltree", size, results);
                memory<cbtree::CBTree<int, 32>>("cbtree", size, results);
            }

            results.finish();

            results.start("memory-big");
            results.set_max(2);

            for(auto size : big_sizes){
                memory<skiplist::SkipList<int, 32>>("skiplist", size, results);
                memory<nbbst::NBBST<int, 32>>("nbbst", size, results);
                memory<lfmst::MultiwaySearchTree<int, 32>>("lfmst", size, results);
                memory<avltree::AVLTree<int, 32>>("avltree", size, results);
                memory<cbtree::CBTree<int, 32>>("cbtree", size, results);
            }

            results.finish();
        } else if(arg == "high"){
            std::cout << "Test the high memory consumption of each version" << std::endl;

            Results results;
            results.start("memory-little-high");
            results.set_max(3);

            for(auto size : little_sizes){
                memory_high<skiplist::SkipList<int, 32>>("skiplist", size, results);
                memory_high<nbbst::NBBST<int, 32>>("nbbst", size, results);
                memory_high<lfmst::MultiwaySearchTree<int, 32>>("lfmst", size, results);
                memory_high<avltree::AVLTree<int, 32>>("avltree", size, results);
                memory_high<cbtree::CBTree<int, 32>>("cbtree", size, results);
            }

            results.finish();

            results.start("memory-big-high");
            results.set_max(2);

            for(auto size : big_sizes){
                memory_high<skiplist::SkipList<int, 32>>("skiplist", size, results);
                memory_high<nbbst::NBBST<int, 32>>("nbbst", size, results);
                memory_high<lfmst::MultiwaySearchTree<int, 32>>("lfmst", size, results);
                memory_high<avltree::AVLTree<int, 32>>("avltree", size, results);
                memory_high<cbtree::CBTree<int, 32>>("cbtree", size, results);
            }

            results.finish();
        } else {
            std::cout << "incorrect argument" << std::endl;
        }
    }

    end = true;

    return 0;
}
