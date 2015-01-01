/* This file is the memory wrapper for the programming language
	it's very simple .. it keeps track of all allocated memory .. 
	and returns them on normal termination of the program */

#include <stdlib.h>
/*
#define MAX_ALLOC 4096
void * _alloc[ MAX_ALLOC ];	
static long unsigned _alloc_total = 0;
int _alloc_head = 0;

void * malloc( size_t size)
{
	_alloc_total += size;
	return _alloc[_alloc_head++] = malloc(size);
}

void * calloc( size_t count , size_t size)
{
	_alloc_total += count * size;
	void * tmp = calloc(count , size);
	_alloc[_alloc_head++] = tmp;
	return tmp;
}

void _free_alloc(void)
{
	//printf("_alloc_head = %lu\n" , _alloc_head );
	while(_alloc_head)
		free(_alloc[_alloc_head--]);
	printf("Total allocated memory %lu Bytes , %lu KB , %lu MB\n" , _alloc_total , _alloc_total/1024 , _alloc_total/1024/1024 );
}
*/