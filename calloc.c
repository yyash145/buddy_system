#include "malloc.h"

void *calloc(size_t count, size_t size) {
	size_t *caloc, size_8;
	caloc = malloc(count*size);
	if (caloc != NULL) {
		size_8 = align8(count * size);
		size_8 = size_8 >> 2;
		memset(caloc, 0, size_8); 
	}
	return caloc;
}
