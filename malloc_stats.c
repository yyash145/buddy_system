#include "malloc.h"

void malloc_stats(void) {
	struct mallinfo info = mallinfo();

	printf("Malloc Statistics:\n");
	printf("Total arenas: %d\n", info.arenas);
	printf("Number of free chunks: %d\n", info.free_chunk);
	printf("Number of mmapped regions: %d\n", info.r_mapped);
	printf("Space allocated in mmapped regions (bytes): %u\n", info.r_mapped_space);
	printf("Maximum total allocated space (bytes): %u\n", info.total_space_max);
	printf("Total allocated space (bytes): %u\n", info.total_space);
	printf("Total free space (bytes): %u\n", info.free_space);

	printf("\nArena Wise Statistics:\n");
	m_info start = arena_head;
	int i = 0;
	while(start != NULL) {
		printf("Arena: %d\n", i);
		printf("Number of free chunks: %d\n", start->free_chunk);
		printf("Number of mapped regions: %d\n", start->r_mapped);
		printf("Space allocated in mapped regions (bytes): %u\n", start->r_mapped_space);
		printf("Maximum total allocated space (bytes): %u\n", start->total_space_max);
		printf("Total allocated space (bytes): %u\n", start->total_space);
		printf("Total free space (bytes): %u\n", start->free_space);
		printf("Total allocation requests: %d\n", start->allocation_req);
		printf("Total free requests: %d\n", start->free_req);
		printf("\n");
		i++;
		start = start->next;
	}
}