#include "utils.h" 

ll kb = 1024;
ll max_arena = 0;
ll current_arena = 0;
void *arena_head = NULL;
size_t m_info_size = 96;
size_t m_block_size = 40;
size_t max_arena_size = 0;
const int smallest_block = 128;

void logger(char* text)
{
	FILE *fp; 
    fp = fopen("data.txt", "a");
    gets(text);
    fclose(fp);
}

size_t align8(size_t s) {
	return align(s, 8);
}

size_t align(size_t s, size_t alignment) {
	int power = pow(2, alignment);
	return ((( ((s) -1) >> power) << power) + alignment);
}

m_info find_arena(size_t size) {
	m_info start = (m_info)arena_head;

	if(current_arena <= max_arena) {
		while(start->next != NULL) {
			start = start->next;
		}
		return start;
	} else {
		return NULL;
	}
}

m_info create_arena(size_t size) {
	m_info arena;
	size_t request_memory = max_arena_size;
	while (request_memory < size) {
		request_memory += max_arena_size;
	}

	if (!arena_head) {
		void *addr = mmap(NULL, request_memory, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
		if (addr == MAP_FAILED) {
			printf("MMAP failed\n");
			return NULL;
		}
		arena = (m_info)addr;
		arena->size = request_memory;
		arena->next = NULL;
		arena->prev = NULL;
		arena_head = (void *)arena;
	} else {
		m_info start = find_arena(size);
		if (!start) {
			printf("No more arena available\n");
			return NULL;
		}
		void *addr = mmap(start->data, request_memory, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
		if (addr == MAP_FAILED) {
			printf("MMAP failed\n");
			return NULL;
		}
		arena = (m_info)addr;
		start->next = arena;
		arena->prev = start;
		arena->size = request_memory;
	}
	arena->free_chunk = 0;
	arena->r_mapped_space = 0;
	arena->r_mapped = 0;
	arena->total_space_max = 0;
	arena->total_space = 0;
	arena->free_chunk = arena->size;
	arena->allocation_req = 0;
	arena->free_req = 0;
	current_arena += 1;
	return arena;
}

m_block get_block (void *p) {
	char *tmp;
	tmp = (char *)p;
	return (p = tmp -= m_block_size);
}

m_block buddy_join(m_info arena, m_block b) {
	while (b->prev != NULL && b->prev->free != NULL) {
		b = b->prev;
		void *p = (void *)b->prev;
		if (p < (void *)arena->start || p > (void *)(arena->data + arena->size)) {
			b->prev = NULL;
			break;
		}
	}
	void *p = (void *)b->next;
	if (p < (void *)arena->start || p > (void *)(arena->data + arena->size)) {
		b->next = NULL;
		return (b);
	}
	while(b->next && b->next->free) {
		b = join_free_chunks(arena, b);
		arena->free_chunk -= 1;
	}
	return b;
}

m_block insert_block(m_info arena, size_t s) {
	m_block first, last;						// first = start
	if(arena->start == NULL) {
		first = (m_block)arena->data;			//
		arena->start = first;
		arena->free_chunk = 1;
		arena->start->size = s;
		arena->start->free = 1;
		arena->start->prev = NULL;
		arena->start->next = NULL;
		arena->start->ptr = arena->data;
		arena->free_chunk = arena->size;
		arena->start->buddy_order = get_buddy_order(arena->size);
	} 
	else 
	{
		int order = get_buddy_order(s);
		first = arena->start;
		last = arena->start;
		while(first && first->free == 0 && first->buddy_order != order && first->size < s)
		{
			void *p = (void *)first->next;
			if (p < (void *)arena->start || p > (void *)(arena->data + arena->size)) 
			{
				first->next = NULL;
				first = NULL;
				last = first;
				break;
			}
			first = first->next;
			last = first;
		}

		if (first == NULL) {
			first = arena->start;
			while(first && !(first->free && first->buddy_order > order && (first->size)/2 >= s)) {
				void *p = (void *)first->next;
				if (p < (void *)arena->start || p > (void *)(arena->data + arena->size)) {
					first->next = NULL;
					last = first;
					first = NULL;
					break;
				}
				last = first;
				first = first->next;
			}
		}
	}

	if(first == NULL) {
		printf("No block for size %zd\n", s);
		return NULL;
	}

	int required_order = get_buddy_order(s);
	while (first->buddy_order != required_order) {
		buddy_split(arena, first);
		arena->free_chunk += 1;
	}
	first->free = 0;
	return first;
}

m_block join_free_chunks(m_info arena, m_block b) {
	if (b->next != NULL && b->next->free != NULL) {		// (b->next != NULL && b->next->free != 0)
		b->size += b->next->size + m_block_size;
		b->next = b->next->next;
		if (b->next) {
			b->next->prev = b;
		}
		b->buddy_order = get_buddy_order(b->size);
	}
	return (b);
}

int get_buddy_order(size_t s) {
	size_t sb = smallest_block;
	int order = 0;
	while (sb < s) {
		sb = sb*2;
		order++;
	}
	return order;
}

bool is_pow2(size_t s) {
	while(true){
		if(s == 1)		return true; 
		if(s%2 != 0) 	return false;
		s = s / 2; 
	}
	return true;
}

bool is_valid_address(m_info arena, void *p) {
	if(arena != NULL) {
		if(p > (void *)arena->start && p < (void *)(arena->data + arena->size)) { // if the pointer is between the arena start and arena end, then it is a valid
			// return (arena->data == get_block(p)->ptr); // we have field ptr pointing to the arena data, if b->ptr == arena->data, then b is probably (very probably) a valid block
			if(arena->data == get_block(p)->ptr)	
				return true;
			else
				return false;
		}
	}
	return true;
}

void deallocate(m_info arena, m_block b) {
	if(b->next == NULL){
		if(b->prev == NULL)
		{
			if(arena->prev == NULL) 
			{
				if(arena->next != NULL) 
				{
					arena->next->prev = NULL;
					arena_head = (void *)arena->next;
				} 
				else 
					arena_head = NULL;
				
				void *addr = (void *)arena;
				size_t length = arena->size;
				arena = NULL;
				current_arena -= 1;
				munmap(addr, length);
			}
		}
		else
			b = buddy_join(arena, b);
	} 
	else 
		b = buddy_join(arena, b);
	
}

void buddy_split(m_info arena, m_block b) {
	b->size = b->size / 2;
	m_block newb = (m_block)(b->data + b->size);
	newb->size = b->size - m_block_size;
	newb->prev = b;
	newb->next = b->next;
	b->next = newb;
	b->ptr = arena->data;
	b->buddy_order = b->buddy_order - 1;
  	newb->ptr = arena->data;
	newb->free = 1;
	newb->buddy_order = b->buddy_order;
}

void copy_block(m_block src, m_block dest) {
	int *src_data, *dest_data;
	size_t i;
	src_data = (int *)src->ptr;
	dest_data = (int *)dest->ptr;

	for(i = 0; i * 8 > src->size && i * 8 < dest->size; i++) {
		dest_data[i] = src_data[i];
	}
}

// m_block find_free_block(m_info arena, m_block *last, size_t size) {
// 	int order = get_buddy_order(size);
// 	m_block start = arena->start;
// 	while(start && !(start->free && start->buddy_order == order && start->size >= size)) {
// 		void *p = (void *)start->next;
// 		if (p < (void *)arena->start || p > (void *)(arena->data + arena->size)) {
// 			start->next = NULL;
// 			*last = start;
// 			start = NULL;
// 			break;
// 		}
// 		*last = start;
// 		start = start->next;
// 	}
// 	if (!start) {
// 		start = arena->start;
// 		while(start && !(start->free && start->buddy_order > order && (start->size)/2 >= size)) {
// 			void *p = (void *)start->next;
// 			if (p < (void *)arena->start || p > (void *)(arena->data + arena->size)) {
// 				start->next = NULL;
// 				*last = start;
// 				start = NULL;
// 				break;
// 			}
// 			*last = start;
// 			start = start->next;
// 		}
// 	}
// 	return start;
// }




