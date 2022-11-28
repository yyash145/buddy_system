#include "malloc.h"

void *memalign(size_t alignment, size_t size) {
	if (is_pow2(alignment) == true) {
		errno = EINVAL;
		return (NULL);
	}
	size_t s = align(size, alignment);
	void *ptr = malloc(s);
	if(!ptr) {
		errno = ENOMEM;
		return (NULL);
	}
	return ptr;
}
