#ifndef MM_H
#define MM_H

#include <stdio.h>

#define EXTROSIZE   16
#define HEADSIZE sizeof(struct block_t)


typedef struct block_t block_s; 
struct block_t {
	int is_avilible;
	size_t block_size;
	block_s *pre;
	block_s *next;
};


extern int   mm_init(void);
extern void  *mm_malloc(size_t size);
extern void  mm_free(void *ptr);
extern void  *mm_realloc(void *ptr, size_t size);


static block_s *find_fit(size_t size);
static void    split(block_s * block, size_t size);
static void    coalesce(block_s *block);
static size_t     align8(size_t size);


//global variables
int  initialized;
void *mm_start;
void *mm_end;


//for debug
static void print_blocks(void);

#endif



