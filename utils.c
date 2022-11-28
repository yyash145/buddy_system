#include "utils.h"

ll kb = 1024;
ll max_arena = 0;
ll current_arena = 0;
void *arena_head = NULL;
size_t m_info_size = 96;
size_t m_block_size = 40;
size_t max_arena_size = 0;
const int smallest_block = 128;

void logger(char *text)
{
	FILE *fp;
	char *filepath = "./data.txt";
	// remove("welcome.txt") 
	fp = fopen(filepath, "a");
	if(fp == NULL){
		printf("Error in file opening\n");
	}
	// int size = sizeof(text);
	fputs(text, fp);
	fclose(fp);
}

int is_two_power(size_t s)
{
	while (s > 1) {
		if (s % 2 != 0) {
			return -1;
		}
		s = s / 2;
	}
	return 0;
}

int get_two_power(size_t s) {
	int power = 0;
	while (s > 1) {
		s = s / 2;
		power++;
	}
	return power;
}

m_info find_arena(size_t size)
{
	m_info start = (m_info)arena_head;

	if (current_arena <= max_arena)
	{
		while (start->next != NULL)
		{
			start = start->next;
		}
		return start;
	}
	else
	{
		return NULL;
	}
}

// m_info find_arena(size_t size) {
// 	m_info start = arena_head;
// 	if(current_arenas <= max_arenas) {
// 		while(start->next != NULL) {
// 			start = start->next;
// 		}
// 		return start;
// 	} else {
// 		return NULL;
// 	}
// }

m_block join_free_chunks(m_info arena, m_block b)
{
	if (b->next != NULL && b->next->free != 0)
	{
		b->size += b->next->size + m_block_size;
		b->next = b->next->next;
		if (b->next != NULL)
			b->next->prev = b;
		b->buddy_order = get_buddy_order(b->size);
	}
	return (b);
}

int get_buddy_order(size_t s)
{
	size_t sb = smallest_block;
	int order = 0;
	while(sb < s)
	{
		sb = 2*sb;
		order++;
	}
	return order;
}

void buddy_split(m_info arena, m_block b)
{
	b->size /= 2;
	m_block newb = (m_block)(b->data + b->size);
	newb->prev = b;
	newb->next = b->next;
	newb->size = b->size - m_block_size;
	b->next = newb;
	b->ptr = arena->data;
	b->buddy_order = b->buddy_order - 1;
	newb->ptr = arena->data;
	newb->free = 1;
	newb->buddy_order = b->buddy_order;
}

void allocate(m_info arena, m_block b, size_t s, m_block last) {
	b->size = s;
	b->prev = last;
	b->next = NULL;
	b->free = 1;
	b->buddy_order = get_buddy_order(s);
	b->ptr = arena->data;
}

void deallocate(m_info arena, m_block b)
{
	if (b->next == NULL)
	{
		if (b->prev == NULL)
		{
			if (arena->prev == NULL)
			{
				if(arena->next == NULL)
					arena_head	= NULL;
				else
				{
					arena->next->prev = NULL;
					arena_head = (void *)arena->next;
				}
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

m_block buddy_join(m_info arena, m_block b)
{
	while (b->prev != NULL && b->prev->free != 0)
	{
		void *p = (void *)b->prev->prev;
		if (p < (void *)arena->start || p > (void *)(arena->data + arena->size))
		{
			b->prev->prev = NULL;
			break;
		}
		b = b->prev;
	}
	void *p = (void *)b->next;
	if (p < (void *)arena->start)
	{
		b->next = NULL;
		return (b);
	}
	else if (p > (void *)(arena->data + arena->size))
	{
		b->next = NULL;
		return (b);
	}
	while (b->next != NULL && b->next->free != 0)
	{
		b = join_free_chunks(arena, b);
		arena->free_chunk -= 1;
	}
	return b;
}

size_t align(size_t s, size_t alignment) {
	int power = get_two_power(alignment);
	return ((( ((s) -1) >> power) << power) + alignment);
}

size_t align8(size_t s) {
	return align(s, 8);
}

// size_t block_size() {
// 	// Since its a 64 bit machine
// 	return 40;
// }

// size_t arena_size() {
// 	// Since its a 64 bit machine
// 	return 96;
// }

m_block get_block(void *p)
{
	char *tmp;
	tmp = (char *)p;
	return (p = tmp -= m_block_size);
}

m_block insert_block(m_info arena, size_t s)
{
	m_block first, last;
	if (arena->start == NULL)
	{
		first = (m_block)arena->data;
		arena->start = first;
		allocate(arena, arena->start, arena->size, NULL);
		// arena->start->size = arena->size;
		// arena->start->free = 1;
		// arena->start->prev = NULL;
		// arena->start->next = NULL;
		// arena->start->ptr = arena->data;
		// arena->start->buddy_order = get_buddy_order(arena->size);
		// arena->start->size = arena->size;
		// arena->start->prev = NULL;
		// arena->start->next = NULL;
		// arena->start->free = 1;
		// arena->start->buddy_order = get_buddy_order(s);
		// arena->start->ptr = arena->data;
		
		arena->free_space = arena->size;
		arena->free_chunk = 1;
	}
	else
	{
		last = arena->start;
		// int order = get_buddy_order(s);
		first = find_free_block(arena, &last, s);
	}

	if (first == NULL)
	{
		printf("No block for size %zd\n", s);
		return NULL;
	}

	int required_order = get_buddy_order(s);
	while (first->buddy_order != required_order)
	{
		buddy_split(arena, first);
		arena->free_chunk += 1;
	}
	first->free = 0;
	return first;
}

// m_block insert_block(m_info arena, size_t s) {
// 	m_block start, last;
// 	if(!arena->start) {
// 		start = (m_block)arena->data;		
// 		arena->start = start;
// 		allocate(arena, arena->start, arena->size, NULL);
// 		arena->free_chunk = 1;
// 		arena->free_space = arena->size;
// 	} 
// 	else {
// 		last = arena->start;
// 		start = find_free_block(arena, &last, s);
// 	}
// 	if(!start) {
// 		printf("No block for size %zd\n", s);
// 		return NULL;
// 	}
// 	split_block(arena, start, s);
// 	start->free = 0;
// 	return start;
// }

m_block find_free_block(m_info arena, m_block *last, size_t s){
	m_block first = arena->start;
	int order = get_buddy_order(s);
	while(first != NULL && !(first->free && first->buddy_order == order && first->size >= s))
	{
		void *p = (void *)first->next;
		if(p > (void *)(arena->data + arena->size)) {
			*last = first;
			first->next = NULL;
			first = NULL;
			break;
		}
		else if(p < (void *)arena->start) {
			*last = first;
			first->next = NULL;
			first = NULL;
			break;
		}
		*last = first;
		first = first->next;
	}
	if(first != NULL)
	{
		// do nothing
	}
	else
	{
		first = arena->start;
		while(first != NULL && !(first->free && first->buddy_order > order && (first->size)/2 >= s))
		{
			void *p = (void *)first->next;
			if (p < (void *)arena->start)
			{
				*last = first;
				first->next = NULL;
				first = NULL;
				break;
			}
			else if (p > (void *)(arena->data + arena->size))
			{
				*last = first;
				first->next = NULL;
				first = NULL;
				break;
			}
			*last = first;
			first = first->next;
		}
	}
	return first;
}


void copy_block(m_block src, m_block dest)
{
	int *src_data, *dest_data;
	size_t i;
	src_data = (int *)src->ptr;
	dest_data = (int *)dest->ptr;

	for (i = 0; src->size < i*8 && i*8 < dest->size; i++)
		dest_data[i] = src_data[i];
	
}

m_info create_arena(size_t size)
{
	size_t req_size = max_arena_size;
	while (req_size < size)
		req_size += max_arena_size;
	
	m_info arena;
	
	if(arena_head) {
		m_info start = find_arena(size);
		void *addr = mmap(start->data, req_size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
		if (addr == MAP_FAILED) {
			printf("MMAP failed\n");
			return NULL;
		}
		if (!start) {
			printf("No more arena available\n");
			return NULL;
		}
		size_t rs = req_size;
		arena = (m_info)addr;
		arena->prev = start;
		start->next = arena;
		arena->size = rs;
	}
	else {
		void *addr = mmap(NULL, req_size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
		if (addr == MAP_FAILED) {
			printf("MMAP failed\n");
			return NULL;
		}
		size_t rs = req_size;
		arena = (m_info)addr;
		arena->next = NULL;
		arena->prev = NULL;
		arena->size = rs;
		arena_head = (void *)arena;
	} 
	arena->r_mapped = 0;
	arena->free_req = 0;
	current_arena += 1;
	arena->free_chunk = 0;
	arena->total_space = 0;
	arena->r_mapped_space = 0;
	arena->allocation_req = 0;
	arena->total_space_max = 0;
	arena->free_space = arena->size;
	return arena;
}

// change
// m_info create_arena(size_t size) {
// 	m_info arena;
// 	size_t request_memory = max_arena_size;
// 	while (request_memory < size) {
// 		request_memory += max_arena_size;
// 	}

// 	if (!arena_head) {
// 		void *addr = mmap(NULL, request_memory, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
// 		if (addr == MAP_FAILED) {
// 			printf("MMAP failed\n");
// 			return NULL;
// 		}
// 		arena = (m_info)addr;
// 		arena->size = request_memory;
// 		arena->next = NULL;
// 		arena->prev = NULL;
// 		arena_head = (void *)arena;
// 	} else {
// 		m_info start = find_arena(size);
// 		if (!start) {
// 			printf("No more arena available\n");
// 			return NULL;
// 		}
// 		void *addr = mmap(start->data, request_memory, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
// 		if (addr == MAP_FAILED) {
// 			printf("MMAP failed\n");
// 			return NULL;
// 		}
// 		arena = (m_info)addr;
// 		start->next = arena;
// 		arena->prev = start;
// 		arena->size = request_memory;
// 	}
// 	arena->free_chunk = 0;
// 	arena->r_mapped_space = 0;
// 	arena->r_mapped = 0;
// 	arena->total_space_max = 0;
// 	arena->total_space = 0;
// 	arena->free_space = arena->size;
// 	arena->allocation_req = 0;
// 	arena->free_req = 0;
// 	current_arena += 1;
// 	return arena;
// }

void split_block(m_info arena, m_block b, size_t s) 
{
	int required_order = get_buddy_order(s);
	while (b->buddy_order != required_order) 
	{
		buddy_split(arena, b);
		arena->free_chunk += 1;
	}
}

bool is_valid_address(m_info arena, void *p) {
	if(arena) {
		if(p > (void *)arena->start && p < (void *)(arena->data + arena->size)) 
			return (arena->data == get_block(p)->ptr); 
		
	}
	return false;
}