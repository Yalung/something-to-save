#include <assert.h>
#include "pool.h"
#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include "list.h"

#define SIZE 1000000

typedef struct tcpm_host {
	int32_t ip;
	list_node_t list;
} tcpm_host_t;

tcpm_host_t *h;
list_head_t head;

static inline unsigned long time_diff_usec(struct timeval *t1,
					   struct timeval *t2)
{

	return ((t2->tv_sec * 1000000 + t2->tv_usec) -
		(t1->tv_sec * 1000000 + t1->tv_usec));

}

int main()
{

	int i;
	uint64_t time;
	struct timeval tv1, tv2;

	INIT_LIST_HEAD(&head);

	pool_t *p = pool_create(sizeof(tcpm_host_t), SIZE);

	gettimeofday(&tv1, NULL);
	for (i = 0; i < SIZE; i++) {
		h = pool_alloc_obj(p);
	}
	gettimeofday(&tv2, NULL);
	time = time_diff_usec(&tv1, &tv2);
	printf("alloc %d times by pool: %ld usec, %ld request/s.\n", SIZE, time,
	       SIZE * (1000000 / time));

	gettimeofday(&tv1, NULL);
	for (i = 0; i < SIZE; i++) {
		h = (tcpm_host_t *) malloc(sizeof(tcpm_host_t));
	}
	gettimeofday(&tv2, NULL);
	time = time_diff_usec(&tv1, &tv2);
	printf("alloc %d times by malloc: %ld usec, %ld request/s.\n", SIZE,
	       time, SIZE * (1000000 / time));

	pool_destory(p);

	p = pool_create(sizeof(tcpm_host_t), SIZE);

	h = pool_alloc_obj(p);
	assert(h);

	for (i = 0; i < SIZE; i++) {
		h = pool_alloc_obj(p);
		pool_free_obj(p, h);
	}

	for (i = 0; i < SIZE - 1; i++)
		assert(pool_alloc_obj(p));

	assert(!pool_alloc_obj(p));

	pool_free_obj(p, h);
	assert(pool_alloc_obj(p) == h);

	pool_destory(p);

	p = pool_create(sizeof(tcpm_host_t), SIZE);

}
