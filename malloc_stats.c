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
 

	char result[50];
	logger("Malloc Statistics:\n");
	sprintf(result, "%d", info.arenas);
	logger("Total arenas: ");
	logger(result);

	memset(result, 0, 50);
	sprintf(result, "%d", info.free_chunk);
	logger("\nNumber of free chunks: ");
	logger(result);

	memset(result, 0, 50);
	sprintf(result, "%d", info.r_mapped);
	logger("\nNumber of mmapped regions: ");
	logger(result);

	memset(result, 0, 50);
	sprintf(result, "%u", info.r_mapped_space);
	logger("\nSpace allocated in mmapped regions (bytes): ");
	logger(result);

	memset(result, 0, 50);
	sprintf(result, "%u", info.total_space_max);
	logger("\nMaximum total allocated space (bytes): ");
	logger(result);

	memset(result, 0, 50);
	sprintf(result, "%u", info.total_space);
	logger("\nTotal allocated space (bytes): ");
	logger(result);

	memset(result, 0, 50);
	sprintf(result, "%u", info.free_space);
	logger("\nTotal free space (bytes): ");
	logger(result);

	logger("\n\nArena Wise Statistics:\n");

	m_info head = arena_head;
	int i = 0;
	while(head != NULL) {
		printf("Arena: %d\n", i);
		printf("Number of free chunks: %d\n", head->free_chunk);
		printf("Number of mapped regions: %d\n", head->r_mapped);
		printf("Space allocated in mapped regions (bytes): %u\n", head->r_mapped_space);
		printf("Maximum total allocated space (bytes): %u\n", head->total_space_max);
		printf("Total allocated space (bytes): %u\n", head->total_space);
		printf("Total free space (bytes): %u\n", head->free_space);
		printf("Total allocation requests: %d\n", head->allocation_req);
		printf("Total free requests: %d\n", head->free_req);
		printf("\n");

	

		memset(result, 0, 50);
		sprintf(result, "%d", i);
		logger("     Arena: " + i);
		logger(result);

		memset(result, 0, 50);
		sprintf(result, "%d", head->free_chunk);
		logger("\nNumber of free chunks: "  );
		logger(result);

		memset(result, 0, 50);
		sprintf(result, "%d",head->r_mapped);
		logger("\nNumber of mapped regions: " );
		logger(result);

		memset(result, 0, 50);
		sprintf(result, "%u", head->r_mapped_space);
		logger("\nSpace allocated in mapped regions (bytes): " );
		logger(result);

		memset(result, 0, 50);
		sprintf(result, "%u",  head->total_space_max);
		logger("\nMaximum total allocated space (bytes): " );
		logger(result);

		memset(result, 0, 50);
		sprintf(result, "%u", head->total_space);
		logger("\nTotal allocated space (bytes): ");
		logger(result);

		memset(result, 0, 50);
		sprintf(result, "%u",head->free_space);
		logger("\nTotal free space (bytes): " );
		logger(result);

		memset(result, 0, 50);
		sprintf(result, "%d", head->allocation_req);
		logger("\nTotal allocation requests: ");
		logger(result);

		memset(result, 0, 50);
		sprintf(result, "%d", head->free_req);
		logger("\nTotal free requests: " );
		logger(result);

		
		logger("\n\n");

		i++;
		head = head->next;
	}
}