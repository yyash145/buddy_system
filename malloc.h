#ifndef MALLOC_H_
#define MALLOC_H_
#include "utils.h"

struct mallinfo {
	int arenas; 		//				// Total arenas 
	int free_chunk; 	// ordblks;   	// Number of free chunks 
	int r_mapped;		// hblks;    	// Number of mmapped regions 
	int r_mapped_space; // hblkhd;   	// Space allocated in mmapped regions (bytes) 
	int total_space_max;// usmblks;   	// Maximum total allocated space (bytes) 
	int total_space;	// uordblks;  	// Total allocated space (bytes) 
	int	free_space;		// fordblks;  	// Total free space (bytes) 
};

void *malloc(size_t size);
void free(void *ptr);
void *calloc(size_t count, size_t size);
void *realloc(void *p, size_t size);
void *reallocarray(void *ptr, size_t nmemb, size_t size);
int posix_memalign(void **memptr, size_t alignment, size_t size);
void *memalign(size_t alignment, size_t size);

struct mallinfo mallinfo();
void malloc_stats(void);

extern __thread m_info arena;

#endif
