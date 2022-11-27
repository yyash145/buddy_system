#ifndef UTILS_H_
#define UTILS_H_ 
#include <math.h>
#include <errno.h>				// 
#include <stdio.h>				// 
#include <stdlib.h>				// 
#include <unistd.h>				// sysconfig
#include <stdint.h>				// 
#include <string.h>				// 
#include <stdbool.h>			// Used for bool
#include <pthread.h>			// thread
#include <sys/mman.h>			// 
#include <sys/types.h>			// 



// #include<ostream>
// #include<sstream>
// #include <fstream>


#define ll long int

typedef struct memory_block *m_block;		// Struct to hold the block information		// memory_block *block;
struct memory_block {
	size_t size; 				// Block size 
	m_block prev;				// Previous block 
	m_block next;				// Next block 
	char data [1];				// Stores pointer of the memory block
	void *ptr;			        // Block's pointer pointing to the arena 
	int free;					// Block is free or not 
	int buddy_order;			// Block's buddy order 
};

typedef struct malloc_arena_info *m_info;	// Struct to hold the arena information		// my_malloc_arena *malloc_arena;
struct malloc_arena_info {
	size_t size;				// Size of the arena 
	m_info prev;    			// Previous arena 
	m_info next;    			// Next arena 
	m_block start;				// Starting block of the arena 
	char data[1];				// Stores pointer of the memory block
	int free_chunk;      		// Number of free chunks 						// ordblks;
	int r_mapped;     			// Number of mmapped regions 					// hblks;   
	int r_mapped_space;   		// Space allocated in mmapped regions (bytes) 	// hblkhd;
	int total_space_max;  		// Maximum total allocated space (bytes) 		// usmblks; 
	int total_space;       		// Total allocated space (bytes) 				// uordblks;  
	int free_space;      		// Total free space (bytes) 					// fordblks;
	int allocation_req;			// 
  	int free_req;				// 
	pthread_mutex_t lock;		// lock the arena in case of allocation and deallocation
};


void log(char* text);
// The function returns a fitting chunk, or NULL if none where found. 
// After the execution, the argument last points to the last visited chunk. 
size_t align8(size_t s);	// 8-byte alignment for every size
size_t align(size_t s, size_t alignment);
m_info find_arena(size_t size);
m_info create_arena(size_t size);
m_block get_block (void *p);
m_block buddy_join(m_info arena, m_block b);
m_block insert_block(m_info arena, size_t s);
m_block join_free_chunks(m_info arena, m_block b);
m_block find_free_block(m_info arena, m_block *last, size_t size);
int get_buddy_order(size_t s);
bool is_pow2(size_t s);
bool is_valid_address(m_info arena, void *p);
void deallocate(m_info arena, m_block b);
void buddy_split(m_info arena, m_block b);
void copy_block(m_block src, m_block dest);
void split_block(m_info arena, m_block b, size_t s);
void allocate(m_info arena, m_block b, size_t s, m_block last);

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
