#include <stdio.h>
#include "mm.h"

typedef struct test test_s;
struct test{
	int a;
	char b[8];
};
int main()
{
	printf("mm_malloc.......\n");
	printf("sizeof: %d\n", sizeof(struct test));
	int *p =  (int *)mm_malloc(sizeof(int));
	*p = 1;
	printf("mm_malloc.......\n");
	test_s *t = (test_s *) mm_malloc(sizeof(struct test));
	test_s *tt = (test_s *) mm_malloc(sizeof(struct test));
	t->a = 18;
	tt->a = 188;
	printf("*p = %d\n", *p);
	printf("t->a = %d\n", t->a);
	
	printf("mm_free.......\n");
	mm_free(t);

	test_s *t1 = (test_s *) mm_malloc(sizeof(struct test));
	t1->a = 10;
	printf("t1->a = %d\n", t1->a);
	mm_free(p);
	mm_free(tt);

	test_s *t2 = (test_s *) mm_malloc(sizeof(struct test));
	t2->a = 1777777;
	printf("t2->a = %d\n", t2->a);
	printf("end....\n");
}
