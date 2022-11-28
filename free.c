#include "malloc.h"

void free(void *p) {
	m_block b;
	if (arena != NULL && is_valid_address(arena, p)) {
		b = get_block(p);
		pthread_mutex_lock(&arena->lock);
		b->free = 1;
		size_t s = b->size;
		deallocate(arena, b);
		arena->r_mapped -= 1;
		arena->free_req += 1;
		arena->free_chunk += 1;
		arena->free_space += s;
		arena->total_space -= s;
		arena->r_mapped_space -= s;
		pthread_mutex_unlock(&arena->lock);
	}
} 
