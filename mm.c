#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include "mm.h"

#define DEBUG



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
#ifdef DEBUG
	printf("[init]\n");
	print_blocks();
#endif
	return 0;
}	

size_t align8(size_t size)
{
	size_t extrobytes = size % 8;
	return (extrobytes==0 ? size : size - extrobytes + 8);
}


//split() doesn't care if block is alloced or not
void split(block_s * block, size_t size)
{
	size_t size_align8 =  align8(size);
	if(block->block_size <= size_align8 + HEADSIZE)
		return;
	
	//build a new block and update the block info
	block_s *new_block = block + (size_align8 / 8);
	new_block->is_avilible = 1;
	new_block->block_size = block->block_size - size_align8 - HEADSIZE;
	new_block->pre = block;
	new_block->next = block->next;
	block->next = new_block;
	block->block_size = size_align8;
#ifdef DEBUG
	printf("[split]\n");
	print_blocks();
#endif
}

block_s *find_fit(size_t size)
{
	block_s *search_addr = ((block_s *) mm_start) + 1;
	if(mm_start == (mm_end + HEADSIZE))
	{
		return NULL;
	}
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
#ifdef DEBUG
		printf("[mm_malloc]\n");
	print_blocks();
#endif
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


#ifdef DEBUG
	printf("[before coalesce]\n");
	print_blocks();
#endif
	if(block->pre->is_avilible != 0 && block->next->is_avilible == 0)
	{
#ifdef DEBUG
		printf("[coalesce] 1 0\n");
#endif
		block->pre->next = block->next;
		block->next->pre = block->pre;
		block->pre->block_size += block->block_size + HEADSIZE;
	}
	else if(block->pre->is_avilible == 0 && block->next->is_avilible != 0)
	{
#ifdef DEBUG
		printf("[coalesce] 0 1\n");
#endif
		block_s *bak = block->next;
		bak->pre->next = bak->next;
		bak->next->pre = bak->pre;
		/*block->next = block->next->next;*/
		/*block->next->next->pre = block;*/
		block->block_size += bak->block_size + HEADSIZE;
	}
	else
	{
#ifdef DEBUG
		printf("[coalesce] 1 1\n");
#endif
		block->pre->next = block->next->next;
		block->next->next->pre = block->pre;
		block->pre->block_size += block->block_size + 
			block->next->block_size + 2*HEADSIZE;
	}
#ifdef DEBUG
	printf("[after coalesce]\n");
	print_blocks();
#endif

}

void *mm_realloc(void *ptr, size_t size)
{
	assert(size > 0);
	if(ptr == NULL)
		return mm_malloc(size);
	block_s *block = (block_s *) (ptr - HEADSIZE);
	size_t block_size = block->block_size;
	if(size == block_size)
		return ptr;
	
	if(size > block_size)
	{
		void *new_ptr = mm_malloc(size);
		if(new_ptr == NULL)
			return NULL;
		memcpy(new_ptr, ptr, block_size);	
		mm_free(ptr);
#ifdef DEBUG
		printf("[realloc]\n");
	print_blocks();
#endif
		return new_ptr;
	}
	else
	{
		split(block, size);
#ifdef DEBUG
		printf("[realloc]\n");
	print_blocks();
#endif
		return ptr;
	}
}

void print_blocks(void)
{
	int index = 0;
	block_s *p = (block_s *) mm_start;
	printf("=============================\n");
	do
	{
		printf("%d : available = %d;  block_size = %d.\n", index++, p->is_avilible, p->block_size);
		p = p->next;
	}
	while(p != NULL);
	printf("=============================\n");
}





