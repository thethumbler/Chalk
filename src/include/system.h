#include "mem.h"

void chalk_exit()
{
	_free_alloc();
	exit(0);
}