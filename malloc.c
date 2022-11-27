#include "malloc.h"

__thread m_info arena = NULL;

void *malloc(size_t size) {
	if (max_arena_size == 0) {					// sysconf - get configuration information at run time
		ll page_size = sysconf(_SC_PAGESIZE);	// Size of a page in bytes. Must not be less than 1.
		max_arena_size = page_size * page_size;
		for(ll i = 0; i < 1000*kb; i++)
			max_arena_size += page_size;
	}
	if (max_arena == 0) 
		max_arena = sysconf(_SC_NPROCESSORS_ONLN);	 // The number of processors currently online (available).
	
	size_t s;
	m_block start;
	s = align8(size); 
	if (arena != NULL) {
		pthread_mutex_lock(&arena->lock); 
		start = insert_block(arena, s);
		pthread_mutex_unlock(&arena->lock); 

		if (start == NULL) 
		{
			errno = ENOMEM; //Error if no more memory can be allocated
 			return(NULL);
	 	}
	} 
	else 
	{
		arena = create_arena(s);
		if (arena == NULL) 
		{ // extending the heap for the first time
			errno = ENOMEM;
			return (NULL);
		}

		pthread_mutex_lock(&arena->lock); //locking the arena before allocation
		start = insert_block(arena, s);
		pthread_mutex_unlock(&arena->lock); //unlocking the arena
		if (start == NULL) 
		{
			errno = ENOMEM; // Error if no more memory can be allocated
	 	 	return(NULL);
	 	}
 	}
	arena->r_mapped += 1;
	arena->free_chunk += 1;
	arena->allocation_req += 1;
	arena->r_mapped_space += s;
	if (s > arena->total_space_max) 
		arena->total_space_max = s;
	
	arena->total_space += s;
	arena->free_space -= s;

 	return(start->data); // returning the starting address of the block
}
