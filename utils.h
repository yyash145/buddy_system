#ifndef UTILS_H_
#define UTILS_H_ 
#include <math.h>				// power
#include <errno.h>				// ENOMEM
#include <stdio.h>				// Standard Input/Output
#include <unistd.h>				// sysconfig
#include <string.h>				// memset
#include <stdbool.h>			// Used for bool
#include <pthread.h>			// thread
#include <sys/mman.h>			// mmap
#include <stdint.h>				// 
#include <stdlib.h>				// 
#include <sys/types.h>			// 
#include <string.h>

#define ll long int

typedef struct memory_block *m_block;
typedef struct malloc_arena_info  *m_info;

struct memory_block {
	size_t size; 				/* Block size */
	m_block next;					/* Next block */
	m_block prev;					/* Previous block */
	int free;					/* Block is free or not */
	int buddy_order;			/* Block's buddy order */
	void *ptr;					/* Block's pointer pointing to the arena */
	char data [1];
};

struct malloc_arena_info  {
	m_block start;				/* Starting block of the arena */
	size_t size;				/* Size of the arena */
	m_info next; 			/* Next arena */
	m_info prev;    		/* Previous arena */
	int free_chunk;      			/* Number of free chunks */			
	int r_mapped;     				/* Number of mmapped regions */
	int r_mapped_space;    				/* Space allocated in mmapped regions (bytes) */
	int total_space_max;   				/* Maximum total allocated space (bytes) */
	int total_space;       		/* Total allocated space (bytes) */
	int free_space;      			/* Total free space (bytes) */
	int allocation_req;
  	int free_req;
	pthread_mutex_t lock;
	char data[1];
};

/* The function returns a fitting chunk, or NULL if none where found. After the execution, the
   argument last points to the last visited chunk. */
void logger(char *text);
int is_two_power(size_t s);
int get_two_power(size_t s);
m_info find_arena(size_t size);
m_block join_free_chunks(m_info arena, m_block b);
int get_buddy_order(size_t s);
void buddy_split(m_info arena, m_block b);
void allocate(m_info arena, m_block b, size_t s, m_block last);
void deallocate(m_info arena, m_block b);
m_block buddy_join(m_info arena, m_block b);
size_t align(size_t s, size_t alignment);
size_t align8(size_t s);
// size_t m_block_size();
// size_t arena_size();
m_block get_block (void *p);
m_block insert_block(m_info arena, size_t s);
m_block find_free_block(m_info arena, m_block *last, size_t size);
void copy_block(m_block src, m_block dest);
m_info create_arena(size_t size);
void split_block(m_info arena, m_block b, size_t s);
bool is_valid_address(m_info arena, void *p);

extern ll kb;
extern ll max_arena;
extern ll current_arena;
extern void *arena_head;
extern size_t m_info_size;
extern size_t m_block_size;
extern size_t max_arena_size;
extern size_t current_map_allocation;		// current_mmap_allocation
extern const int smallest_block;			// SMALLEST_BLOCK

#endif
