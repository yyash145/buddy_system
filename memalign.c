#include "malloc.h"

void *memalign(size_t alignment, size_t size) {
	if (is_two_power(alignment) != 0) {
		errno = EINVAL;
		return (NULL);
	}
	size_t s = align(size, alignment);
	void *ptr = malloc(s);
	if(ptr == NULL) {
		errno = ENOMEM;
		return (NULL);
	}
	return ptr;
}
