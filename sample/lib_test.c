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
	
	for (i = 0; i < numData; i++) 
		MAP_INSERT(cbtreePtr, i+1);

	for (i = 0; i < numData; i++)
		printf("%ld: %d\n", i+1, MAP_CONTAINS(cbtreePtr, i+1)); 

	for (i = 0; i < numData; i++)
		MAP_REMOVE(cbtreePtr, i+1);
		
	for (i = 0; i < numData; i++)
		printf("%ld: %d\n", i+1, MAP_CONTAINS(cbtreePtr, i+1)); 
	
	MAP_FREE(cbtreePtr);
	
	puts("Done.");

	return 0; 
}

