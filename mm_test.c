ear
include <stdio.h>
#include "mm.h"

typedef struct test test_s;
struct test{
	int a;
	char b[8];
};
int main()
{
	printf("sizeof: %d\n", sizeof(struct test));
	int *p =  (int *)mm_malloc(sizeof(int));
	*p = 1;
	test_s *t = (test_s *) mm_malloc(sizeof(struct test));
	test_s *tt = (test_s *) mm_malloc(sizeof(struct test));
	t->a = 18;
	tt->a = 188;
	
	mm_free(t);

	test_s *t1 = (test_s *) mm_malloc(sizeof(struct test));
	t1->a = 10;
	mm_free(p);
	mm_free(tt);

	test_s *t2 = (test_s *) mm_malloc(sizeof(struct test));
	t2->a = 1777777;
}
