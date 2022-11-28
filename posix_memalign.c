#include "malloc.h"

#define VOID_SIZE (sizeof(void *))

int posix_memalign(void **memptr, size_t alignment, size_t size) {
	if (size == 0 || is_pow2(alignment) != 0 || alignment % VOID_SIZE != 0) {
		*memptr = NULL;
		return EINVAL;
	}
	size_t s = align(size, alignment);
	*memptr = malloc(s);
	if (*memptr != NULL) {
 		return (0); // returning the starting address of the block
	}
	return ENOMEM;
}
