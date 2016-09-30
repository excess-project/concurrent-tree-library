EXCESS Concurrent Search Tree Libraries
====

This repository contains the EXCESS’s locality-aware and energy- efficient concurrent search trees: DeltaTree and GreenBST, and EXCESS's experimental work-free and locality-aware concurrent search tree (BlueBST).

This repository also contains various prominent and state-of-the-art concurrent search tree implementations, such as: the concurrent B-tree; lock- and transaction-based dynamic cache-oblivious search trees; the non-blocking binary search trees; Read Copy Update (RCU)-based concurrent search tree; the portably scalable concurrent search tree; transaction-based red-black tree (RBtree), AVL tree (AVLtree), and speculation-friendly tree (SFtree) from Synchrobench (https://github.com/gramoli/synchrobench).

# Licenses

* BlueBST, DeltaTree, and GreenBST are developed by UiT and licensed under the Apache License, Version 2.0;

* CBTree, VTMTree, and SVEB are developed by UiT based on the others’ work. They are licensed under the The GNU General Public License v3.0;

* Other trees are developed by their respective authors and retain their original licenses

# The available trees

### 1. DeltaTree and GreenBST
DeltaTree is a *fine-grained* locality-aware tree. GreenBST is a more compact (in tems of memory footprint) and more optimized variant of DeltaTree. DeltaTree and GreenBST are portable, namely it can maintain their locality-awareness on
different computing platforms (platform-independent).

**Also available in:**
https://github.com/uit-agc/GreenBST

**Related publications:**

* Ibrahim Umar, Otto J. Anshus, and Phuong H. Ha. GreenBST: An energy-efficient concurrent search tree. Proceedings of the 22nd International European Conference on Parallel and Distributed Computing (Euro-Par ’16), 2016, LNCS, pp. 502-517, Springer.
* Ibrahim Umar, Otto J. Anshus, and Phuong H. Ha. Effect of portable fine-grained locality on energy efficiency and performance in concurrent search trees. Proceedings of the 21st ACM SIGPLAN Symposium on Principles and Practice of Parallel Programming (PPoPP ’16), 2016, pp. 36:1-36:2, ACM.
* Ibrahim Umar, Otto Johan Anshus, and Phuong Hoai Ha. DeltaTree: A Locality-aware Concurrent Search Tree. In Proceedings of the 2015 ACM SIGMETRICS International Conference on Measurement and Modeling of Computer Systems (SIGMETRICS '15), 2015, pp. 457-458, ACM.

### 2. Concurrent B-tree (CBTree)
CBTree is a prominent locality-aware concurrent B+tree [25]. CBTree is a representation of the classic coarse-grained locality-aware search concurrent trees that are usually platform-dependent. CBTree can only perform well if their node size is set correctly (e.g., to the system’s page size). This tree is also often referred as the B-link tree.

**Related publication:**

Philip L. Lehman and s. Bing Yao. 1981. Efficient locking for concurrent operations on B-trees. ACM Trans. Database Syst. 6, 4 (December 1981), 650-670.

### 3. Lock-based (SVEB) and transactional (VTMtree) dynamic cache-oblivious tree

SVEB and VTMtree are the concurrent implementation of the fine-grained locality- aware vEB binary search tree. SVEB uses a global mutex to serialize its concurrent tree operations, while VTMtree uses the transactional memory runtime of the GNU C Compiler.

**Related publication:**

Gerth Stølting Brodal, Rolf Fagerberg, and Riko Jacob. Cache oblivious search trees via binary trees of small height. In Proc. 13th ACM-SIAM Symp. Discrete algorithms, SODA ’02, pages 39–48, 2002.

### 4. Original non-blocking binary search tree (NBBST) and its improved variant (LFBST)

NBBST is the Ellen et al. non-blocking binary search tree implementation. LFBST is the improved variant of the original non-blocking binary search tree. These non-blocking search trees are locality-oblivious.

**NBBST official repository:**
https://github.com/wichtounet/btrees

**LFBST official repository:**
https://github.com/anataraja/lfbst

**Related publications:**

* Aravind Natarajan and Neeraj Mittal. Fast concurrent lock-free binary search trees. In Proc. 19th ACM SIGPLAN Symposium on Principles and Practice of Parallel Programming, PPoPP ’14, pages 317–328, 2014.

* Faith Ellen, Panagiota Fatourou, Eric Ruppert, and Franck van Breugel. Non- blocking binary search trees. In Proc. 29th ACM SIGACT-SIGOPS Symp. Principles of distributed computing, PODC ’10, pages 131–140, 2010.

### 5. Transactional red-black tree (RBtree), AVL tree (AVLtree), and speculation-friendly tree (SFtree) from Synchrobench

Synchrobench is a micro-benchmark suite used to evaluate synchronization techniques on data structures and it contains several state-of-the-art concurrent trees such as red-black tree (developed by Oracle labs), AVLtree (developed by Stanford), and speculation-friendly tree implementation in C. These transactional trees are locality-oblivious.

**Syncrobench official repository:**
https://github.com/gramoli/synchrobench

**Related publications:**

* Vincent Gramoli. More than you ever wanted to know about synchronization: Synchrobench, measuring the impact of the synchronization on concurrent algorithms. In Proceedings of the 20th ACM SIGPLAN Symposium on Principles and Practice of Parallel Programming, PPoPP 2015, pages 1–10, 2015.

* Tyler Crain, Vincent Gramoli, and Michel Raynal. A speculation-friendly binary search tree. In Proc. 17th ACM SIGPLAN Symp. Principles and Practice of Parallel Programming, PPoPP ’12, pages 161–170, 2012.

* Rudolf Bayer. Symmetric binary b-trees: Data structure and maintenance algorithms. Acta Informatica, 1(4):290–306, 1972.

* G. M. Skii and Ye. M. Landis. An algorithm for the organization of information. Doklady Akad. Nauk SSSR, 1962.

### 6. RCU-based concurrent search tree (Citrus)
Citrus is a concurrent binary search tree that utilizes Read-Copy-Update (RCU) synchronization and fine-grained locking for synchronization among updaters. Citrus contain operation is wait-free. This concurrent search tree is locality-oblivious.

**Citrus official repository:**
https://bitbucket.org/mayaarl/citrus

**Related publication:**

Maya Arbel and Hagit Attiya. Concurrent updates with rcu: Search tree as an example. In Proc. 2014 ACM Symposium on Principles of Distributed Computing, PODC ’14, pages 196–205. ACM, 2014.

### 7. Portably scalable concurrent search tree (BSTTK)
BST-TK is the state-of- the-art lock-based concurrent search tree based on the asynchronous concurrency paradigm. BST-TK is portably scalable, namely it scales across different types of hardware platforms. BSTTK is a locality-oblivious tree.

**BSTTK official repository:**
https://github.com/LPD-EPFL/ASCYLIB

**Related publication:**

Tudor David, Rachid Guerraoui, and Vasileios Trigonakis. Asynchronized concurrency: The secret to scaling concurrent search data structures. In Proc. 12th Intl. Conf. on Architectural Support for Programming Languages and Operating Systems, ASPLOS’15, pages 631–644, 2015

# Using the trees

### Library usage example

The concurrent search trees inside this repository can be used as a statically-linked library. However, only BlueBST, DeltaTree, GreenBST, and CBTree that currently provide complete interfaces. These trees provide a (.h) header file that contains public- callable functions of the commonly used tree functions such as: MAP INSERT(map, key), MAP REMOVE(map, key), and MAP CONTAINS(map, key). The code below shows a C program that uses the concurrent Btree (CBTree) as a library.

```C
#define MAP_USE_CBTREE //Use the CBTree

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "map_select.h"

int main ()
{
  long numData = 10;
  long i;
  puts("Starting...");
  MAP_T cbtreePtr = MAP_ALLOC(0, 0);
  assert(cbtreePtr);

  for (i = 0; i < numData; i++){
    MAP_INSERT(cbtreePtr, i+1);
  }

  for (i = 0; i < numData; i++){
    printf("%ld: %d\n", i+1, MAP_CONTAINS(cbtreePtr, i+1));
  }

  for (i = 0; i < numData; i++){
    MAP_REMOVE(cbtreePtr, i+1);
  }

  for (i = 0; i < numData; i++) {
    printf("%ld: %d\n", i+1, MAP_CONTAINS(cbtreePtr, i+1));
  }

  MAP_FREE(cbtreePtr);

  puts("Done.");
  return 0;
}
```

A sample code and its accompanying makefile is available in the `sample/` directory.

### Running the concurrent search trees benchmarks

#### Running all benchmarks
1. Clone the repository
2. Go to `./bench` directory
3. Generate the search tree binaries. Run `./make-bins.sh`
4. Run `qsub ./bench-short` for short benchmark OR `qsub ./bench-long` for complete benchmark (>2 hours).
5. Combined data in CSV format will be available in `./bench/combined` directory
6. Comparison chart in PDF format will be generated in `./bench/charts` directory

#### Running individual benchmark

DeltaTree library also provides a collection of standalone benchmark programs for each tree. Below is an example of running a standalone benchmark program. These benchmark programs accept several runtime parameters (e.g., initial tree size, how many update/search operation ratios).

```
$ ./DeltaTree -h
DeltaTree v0.1
===============
Use -h switch for help.

Accepted parameters
-r <NUM> : Range size
-u <0..100> : Update ratio. 0 = Only search; 100 = Only updates
-i <NUM> Initial tree size
-t <NUM> Triangle (DeltaNode/GNode) size
-n <NUM> Number of threads
-s <NUM> Random seed. 0=time()
-d <0..1> Density
-v <0 or 1> : Valgrind mode (less stats). 0 = False; 1 = True
-h : This help

Benchmark output format:
"0: range, insert ratio, delete ratio, #threads, attempted insert, attempted delete, attempted search, effective insert, effective delete, effective search, time (in msec)"

$ ./DeltaTree -r 5000000 -u 10 -i 1024000 -n 10 -s 0
DeltaTree v0.1
===============

Use -h switch for help.

Parameters:
- Range size r: 5000000
- DeltaNode size t: 127
- Update rate u: 10%
- Number of threads n: 10
- Initial tree size i: 1024000
- Random seed s: 0
- Density d: 0.500000
- Valgrind mode v: 0

Finished building initial DeltaTree
The node size is: 25 bytes
Now pre-filling 1024000 random elements...
...Done!

Finished init a DeltaTree using DeltaNode size 127, with initial 1024000 members

#TS: 1421050928, 511389
Starting benchmark...

0: 5000000, 5.00, 5.00, 10, 249410, 248857, 4501733, 195052, 53720, 1000568, 476

Active (alloc'd) triangle:258187(266398), Min Depth:12, Max Depth:30
Node Count:1165332, Node Count(MAX): 1217838, Rebalance (Insert) Done: 234, Rebalance (Delete) Done: 0, Merging Done: 1
Insert Count:195052, Delete Count:53720, Failed Insert:54358, Failed Delete:195137
Entering top: 0, Waiting at the top:0
```
