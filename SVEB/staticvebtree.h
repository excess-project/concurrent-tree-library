/*
 main.c

 (global lock-based) STATIC IMPLICIT VEB Tree

 Based on the code from:
 G. S. Brodal, R. Fagerberg, and R. Jacob, “Cache oblivious search trees via binary trees of small height,”
 in Proceedings of the thirteenth annual ACM-SIAM symposium on Discrete algorithms, ser. SODA ’02, 2002, pp. 39–48.

 Copyright belongs to the authors.

 */


#ifndef staticveb_h
#define staticveb_h

typedef int domain;

extern domain *val;

int init_tree(int t);
int insert(int ky);
int search_test(domain key);
int delete_node(int ky);
void initial_add (int num, int range);

#endif
