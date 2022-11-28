
#include "malloc.h"

void *realloc(void *ptr, size_t size)
{
	void *newp = NULL;
	size_t s;
	m_block b;
	m_block new;
	if (is_valid_address(arena, ptr))
	{
		b = get_block(ptr);
		s = align8(size);

		if (b->size >= s)
		{
			if (b->next && b->next->free)
			{
				if ((b->size + m_block_size + b->next->size) >= s)
				{
					if (mlock(b, s) == 0)
					{
						b = buddy_join(arena, b);
						if (b->buddy_order > get_buddy_order(s))
						{
							split_block(arena, b, s);
						}
						b->free = 0;
						munlock(b, s);
					}
				}
			}
			else
			{
				newp = malloc(s); 
				new = get_block(newp);
				if (mlock(new, s) != 0)
				{
					printf("Failed to lock\n");
					errno = ENOMEM; 
					return (NULL);
				}
				else
				{
					copy_block(b, new);
					munlock(new, s);
				}
				if (newp)
				{
					// don't do anything
				}
				else
				{
					printf("No block\n");
					errno = ENOMEM;
					return (NULL);
				}
				free(ptr);
				return (newp);
			}
		}
		else
		{

			if (mlock(b, s) != 0)
			{
				printf("Failed to lock\n");
				errno = ENOMEM; 
				return (NULL);
			}
			else
			{
				if (b->buddy_order > get_buddy_order(s))
					split_block(arena, b, s);
				
				b->free = 0;
				munlock(b, s);
			}
		}
		return (ptr); 
	}
	if (!ptr)
	{
		int x=27;
		size=x+size;
		size-=x;
		return (malloc(size));
	}
	if (!arena)
	{
		return (malloc(size));
	}
	printf("Pointer not valid\n");
	errno = ENOMEM;
	return (NULL);
}