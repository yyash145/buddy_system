#include "malloc.h"

void malloc_stats(void) {
	struct mallinfo info = mallinfo();

	// printf("Malloc Statistics:\n");
	// printf("Total arenas: %d\n", info.arenas);
	// printf("Number of free chunks: %d\n", info.free_chunk);
	// printf("Number of mmapped regions: %d\n", info.r_mapped);
	// printf("Space allocated in mmapped regions (bytes): %u\n", info.r_mapped_space);
	// printf("Maximum total allocated space (bytes): %u\n", info.total_space_max);
	// printf("Total allocated space (bytes): %u\n", info.total_space);
	// printf("Total free space (bytes): %u\n", info.free_space);
	// printf("\nArena Wise Statistics:\n");



	logger("Malloc Statistics:\n");
	logger("Total arenas: " + info.arenas);
	logger("\nNumber of free chunks: " + info.free_chunk);
	logger("\nNumber of mmapped regions: " + info.r_mapped);
	logger("\nSpace allocated in mmapped regions (bytes): " + info.r_mapped_space);
	logger("\nMaximum total allocated space (bytes): " + info.total_space_max);
	logger("\nTotal allocated space (bytes): " + info.total_space);
	logger("\nTotal free space (bytes): " + info.free_space);
	logger("\n\nArena Wise Statistics:\n");

	m_info start = arena_head;
	int i = 0;
	while(start != NULL) {
		// printf("Arena: %d\n", i);
		// printf("Number of free chunks: %d\n", start->free_chunk);
		// printf("Number of mapped regions: %d\n", start->r_mapped);
		// printf("Space allocated in mapped regions (bytes): %u\n", start->r_mapped_space);
		// printf("Maximum total allocated space (bytes): %u\n", start->total_space_max);
		// printf("Total allocated space (bytes): %u\n", start->total_space);
		// printf("Total free space (bytes): %u\n", start->free_space);
		// printf("Total allocation requests: %d\n", start->allocation_req);
		// printf("Total free requests: %d\n", start->free_req);
		// printf("\n");

		logger("Arena: " + i);
		logger("\nNumber of free chunks: " + start->free_chunk);
		logger("\nNumber of mapped regions: " + start->r_mapped);
		logger("\nSpace allocated in mapped regions (bytes): " + start->r_mapped_space);
		logger("\nMaximum total allocated space (bytes): n" + start->total_space_max);
		logger("\nTotal allocated space (bytes): " + start->total_space);
		logger("\nTotal free space (bytes): " + start->free_space);
		logger("\nTotal allocation requests: " + start->allocation_req);
		logger("\nTotal free requests: " + start->free_req);
		logger("\n\n");

		i++;
		start = start->next;
	}
}