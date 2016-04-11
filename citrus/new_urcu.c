#include <stdlib.h>
#include "urcu.h"
#include <stdio.h>
#include <assert.h>

/**
 * Copyright 2014 Maya Arbel (mayaarl [at] cs [dot] technion [dot] ac [dot] il).
 * 
 * This file is part of Citrus. 
 * 
 * Citrus is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * Authors Maya Arbel and Adam Morrison 
 */

int threads; 
rcu_node** urcu_table;

void initURCU(int num_threads){
   rcu_node** result = (rcu_node**) malloc(sizeof(rcu_node)*num_threads);
   int i;
   rcu_node* NEW;
   threads = num_threads; 
   for( i=0; i<threads ; i++){
        NEW = (rcu_node*) malloc(sizeof(rcu_node));
        NEW->time = 1; 
        *(result + i) = NEW;
    }
    urcu_table =  result;
    printf("initializing URCU finished, node_size: %zd\n", sizeof(rcu_node));
    return; 
}

__thread long* times = NULL; 
__thread int i; 

void urcu_register(int id){
    times = (long*) malloc(sizeof(long)*threads);
    i = id; 
    if (times == NULL ){
        printf("malloc failed\n");
        exit(1);
    }
}
void urcu_unregister(){
    free(times);
}

void urcu_read_lock(){
    assert(urcu_table[i]!= NULL);
    __sync_add_and_fetch(&urcu_table[i]->time, 1);
}

static inline void set_bit(int nr, volatile unsigned long *addr){
#ifdef __arm__
do{
	u_int cpsr_save, tmp;

    	__asm volatile(					\
                        "mrs  %0, cpsr;"                \
                         "orr  %1, %0, %2;"             \
                         "msr  cpsr_all, %1;"           \
                         : "=r" (cpsr_save), "=r" (tmp) \
                         : "I" (1 << 7)         	\
                         : "cc" );               	\
	*addr |= (1 << nr);
	__asm volatile(                			\
                         "msr  cpsr_all, %0"     	\
                         : /* no output */       	\
                         : "r" (cpsr_save)       	\
                         : "cc" );               	\
} while(0);
#else
    asm("btsl %1,%0" : "+m" (*addr) : "Ir" (nr));
#endif
}

void urcu_read_unlock(){
    assert(urcu_table[i]!= NULL);
    set_bit(0, &urcu_table[i]->time);
}

void urcu_synchronize(){
    int i; 
    //read old counters
    for( i=0; i<threads ; i++){
        times[i] = urcu_table[i]->time;
    }
    for( i=0; i<threads ; i++){
        if (times[i] & 1) continue;
        while(1){
            unsigned long t = urcu_table[i]->time;
            if (t & 1 || t > times[i]){
                break; 
            }
        }
    }
} 
