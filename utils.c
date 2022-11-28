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
	fp = fopen("data.txt", "a");
	gets(text);
	fclose(fp);
}

size_t align8(size_t s)
{
	return align(s, 8);
}

size_t align(size_t s, size_t alignment)
{
	int power = pow(2, alignment);
	return (((((s)-1) >> power) << power) + alignment);
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

m_info create_arena(size_t size)
{
	size_t req_size = max_arena_size;
	while (req_size < size)
	{
		req_size += max_arena_size;
	}

	m_info arena;
	if (arena_head != NULL)
	{
		m_info start = find_arena(size);
		if (start == NULL)
		{
			printf("No more arena available\n");
			return NULL;
		}
		void *addr = mmap(start->data, req_size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
		if (addr == MAP_FAILED)			// map or unmap files or devices into memory
		{
			printf("Mapping failed\n");
			return NULL;
		}
		int rs=req_size;
		int bud_chain=0;
		arena->size = rs;
		arena = (m_info)addr;
		start->next = arena;
		arena->prev = start;
	}
	else
	{
		void *addr = mmap(NULL, req_size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
		if (addr == MAP_FAILED)
		{
			printf("MMAP failed\n");
			return NULL;
		}
		arena = (m_info)addr;
		arena_head = (void *)arena;
		arena->size = req_size;
		arena->prev = NULL;
		arena->next = NULL;
	}
	arena->r_mapped = 0;
	arena->free_req = 0;
	current_arena += 1;
	arena->free_chunk = 0;
	arena->total_space = 0;
	arena->r_mapped_space = 0;
	arena->allocation_req = 0;
	arena->total_space_max = 0;
	arena->free_chunk = arena->size;
	return arena;
}

m_block get_block(void *p)
{
	char *tmp;
	tmp = (char *)p;
	return (p = tmp -= m_block_size);
}

m_block buddy_join(m_info arena, m_block b)
{
	while (b->prev != NULL && b->prev->free != NULL)
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
	while (b->next != NULL && b->next->free != NULL)
	{
		b = join_free_chunks(arena, b);
		arena->free_chunk -= 1;
	}
	return b;
}

m_block insert_block(m_info arena, size_t s)
{
	m_block first, last;
	if (arena->start == NULL)
	{
		first = (m_block)arena->data; //
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
		while (first != NULL && first->free == 0 && first->buddy_order != order && first->size < s)
		{
			void *p = (void *)first->next;
			if (p > (void *)(arena->data + arena->size) || p < (void *)arena->start)
			{
				first->next = NULL;
				first = NULL;
				last = first;
				break;
			}
			first = first->next;
			last = first;
		}
		if (first == NULL)
		{
			// do nothing
		}
		else
		{
			first = arena->start;
			int div = (first->size) / 2;
			while (first != NULL && first->free == 0 && first->buddy_order <= order && div < s)
			{
				void *p = (void *)first->next;
				if (p < (void *)arena->start)
				{
					first->next = NULL;
					last = first;
					first = NULL;
					break;
				}
				else if (p > (void *)(arena->data + arena->size))
				{
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

m_block join_free_chunks(m_info arena, m_block b)
{
	if (b->next != NULL && b->next->free != NULL)
	{
		b->size += b->next->size + m_block_size;
		b->next = b->next->next;
		if (b->next != NULL)
			b->next->prev = b;
		b->buddy_order = get_buddy_order(b->size);
	}
	return (b);
}

bool is_pow2(size_t s)
{
	while (true)
	{
		if (s == 1)
			return true;
		if (s % 2 != 0)
			return false;
		s = s / 2;
	}
	return true;
}

bool is_valid_address(m_info arena, void *p)
{
	if (arena != NULL)
	{
		if (p > (void *)arena->start && p < (void *)(arena->data + arena->size))
		{ // if the pointer is between the arena start and arena end, then it is a valid
			if (arena->data == get_block(p)->ptr)
				return true;
			else
				return false;
		}
	}
	return true;
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

void buddy_split(m_info arena, m_block b)
{
	b->size /= b->size;
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

void copy_block(m_block src, m_block dest)
{
	int *src_data, *dest_data;
	size_t i;
	src_data = (int *)src->ptr;
	dest_data = (int *)dest->ptr;

	for (i = 0; src->size < i*8 && i*8 < dest->size; i++)
		dest_data[i] = src_data[i];
	
}
