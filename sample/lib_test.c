#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "map_select.h"

int main ()
{
	long numData = 100;
	long i;

	puts("Starting...");

	MAP_T cbtreePtr = MAP_ALLOC(0, 0);
	assert(cbtreePtr);

	for (i = 0; i < numData; i++){
		char *a = malloc (sizeof(char));
		*a = 'A' + (i%56);
		MAP_INSERT(cbtreePtr, i+1, a);
	}

	for (i = 0; i < numData; i++)
		printf("%ld: %d\n", i+1, MAP_CONTAINS(cbtreePtr, i+1));

	for (i = 0; i < numData; i++){
		char *a = (char*) MAP_GET(cbtreePtr, i+1);
		if(!a)
			printf("Error getting value for key %ld!\n", i+1);
		else
			printf("key %ld: , value %c\n", i+1, *a);
	}

	for (i = 0; i < numData; i++)
		MAP_REMOVE(cbtreePtr, i+1);

	for (i = 0; i < numData; i++)
		printf("%ld: %d\n", i+1, MAP_CONTAINS(cbtreePtr, i+1));

	MAP_FREE(cbtreePtr);

	puts("Done.");

	return 0; 
}

