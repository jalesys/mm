#include <unistd.h>
#include <string.h>
#include "mm.h"

int mm_init(void)
{
	block_s *head_block, *tail_block;
	initialized = 0;
	mm_start = sbrk(0);
	if(mm_start == (void*)-1)
		return -1;
	if(sbrk(2*HEADSIZE) == (void *) -1)
		return -1;
	head_block = (block_s *) mm_start;
	mm_end = mm_start + HEADSIZE;
	tail_block = (block_s *) mm_end;

	head_block->is_avilible = 0;
	head_block->block_size = 0;
	head_block->pre = NULL;
	head_block->next = tail_block;

	tail_block->is_avilible = 0;
	tail_block->block_size = 0;
	tail_block->pre = head_block;
	tail_block->next = NULL;
	initialized = 1;
	return 0;
}	

size_t align8(size_t size)
{
	size_t extrobytes = size % 8;
	return (extrobytes==0 ? size : size - extrobytes + 8);
}

void split(block_s * block, size_t size)
{
	if(block->block_size <= size)
		return;
	
	//build a new block and update the block info
	block_s *new_block = block + (size / 8);
	new_block->is_avilible = 1;
	new_block->block_size = block->block_size - size - HEADSIZE;
	new_block->pre = block;
	new_block->next = block->next;
	block->next = new_block;
}

block_s *find_fit(size_t size)
{
	block_s *search_addr = ((block_s *) mm_start) + 1;
	if(mm_start == (mm_end + HEADSIZE))
		return NULL;
	while(search_addr != mm_end)
	{
		if(search_addr->is_avilible == 1&& 
				search_addr->block_size >= size)
		{
			if(search_addr->block_size >= 3*size)
				split(search_addr, size);
			return search_addr;
		}
		search_addr = search_addr->next;
	}
	return NULL;
}

void *mm_malloc(size_t size)
{
	size_t size_align8 = align8(size);
	if(initialized == 0)
		if(mm_init() == -1)
			return NULL;
	
	block_s *block = find_fit(size_align8);
	if(block == NULL)
	{
		block_s *tail_block;
		block_s *last_block = ((block_s *) mm_end)->pre;
		if(sbrk(size_align8 + HEADSIZE) == (void *) -1)
			return NULL;
		block = (block_s *) mm_end;
		mm_end += size_align8 + HEADSIZE;
		tail_block = (block_s *) mm_end;

		block->is_avilible = 0;
		block->block_size = size_align8;
		block->pre = last_block;	
		block->next = tail_block;
		last_block->next = block;
	
		tail_block->is_avilible = 0;
		tail_block->block_size = 0;
		tail_block->pre = block;
		tail_block->next = NULL;
	}
	return (void *)block + HEADSIZE;
}

void mm_free(void *ptr)
{
	block_s *block = (block_s *) (ptr - HEADSIZE);
	ptr = NULL;
	block->is_avilible = 1;
	coalesce(block);
}

void coalesce(block_s *block)
{
	if(block == NULL)
		return;

	if(block->pre->is_avilible == 0 && block->next->is_avilible == 0)
		return;

	if(block->pre->is_avilible != 0 && block->next->is_avilible == 0)
	{
		block->pre->next = block->next;
		block->pre->block_size += block->block_size + HEADSIZE;
	}
	else if(block->pre->is_avilible == 0 && block->next->is_avilible != 0)
	{
		block->next = block->next->next;
		block->block_size += block->next->block_size + HEADSIZE;
	}
	else
	{
		block->pre->next = block->next->next;
		block->pre->block_size += block->block_size + 
			block->next->block_size + 2*HEADSIZE;
	}

}










