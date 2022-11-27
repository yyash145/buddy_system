#include "malloc.h"

void malloc_stats(void) {
	struct mallinfo info = mallinfo();
	printf("Malloc Statistics:\n");
	printf("Total arenas: %d\nNumber of free chunks: %d\nNumber of mmapped regions: %d\nSpace allocated in mmapped regions (bytes): %u\nMaximum total allocated space (bytes): %u\nTotal allocated space (bytes): %u\nTotal free space (bytes): %u\n\n", info.arenas, info.free_chunk, info.r_mapped, info.r_mapped_space, info.total_space_max, info.total_space, info.free_space);

	printf("Arena Wise Statistics:\n");
	m_info start = arena_head;
	int i = 0;
	while(start != NULL) {
		printf("Arena: %d\n", i);
		printf("Number of free chunks: %d\nNumber of mmapped regions: %d\nSpace allocated in mmapped regions (bytes): %u\nMaximum total allocated space (bytes): %u\nTotal allocated space (bytes): %u\nTotal free space (bytes): %u\nTotal allocation requests: %d\nTotal free requests: %d\n\n", start->free_chunk, start->r_mapped, start->r_mapped_space, start->total_space_max, start->total_space, start->free_space, start->allocation_req, start->free_req);
		i++;
		start = start->next;
	}
}
