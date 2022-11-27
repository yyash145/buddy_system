#include "malloc.h"

struct mallinfo mallinfo() {
	struct mallinfo info;
	info.arenas = current_arena;
	info.free_chunk = 0;
	info.r_mapped = 0;
	info.r_mapped_space = 0;
	info.total_space_max = 0;
	info.total_space = 0;
	info.free_space = 0;

	m_info start = arena_head;
	while(start != NULL) {
		info.free_chunk += arena->free_chunk;
		info.r_mapped += arena->r_mapped;
		info.r_mapped_space += arena->r_mapped;
		if (arena->total_space_max > info.total_space_max)
			info.total_space_max += arena->total_space_max;
		info.total_space += arena->total_space;
		info.free_space += arena->free_space;
		start = start->next;
	}

	return info;
}
