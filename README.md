something-to-save
=================

#include "ring.h"
#include <pthread.h>
#include <assert.h>

#define PTR(i) ((void*)(i))

#define SIZE 1024

ring_t *r;

void *consumer(void *arg)
{
	UNUSED(arg);
	while (1) {
		int *i = ring_dequeue(r);
		if (i)
			assert(*i == 100);
		free(i);
	}
}

int main()
{
	unsigned long i;
	pthread_t tid;

	assert(!ring_create(SIZE - 1));

	r = ring_create(SIZE);

	assert(!ring_dequeue(r));
	assert(ring_empty(r));
	assert(!ring_full(r));

	ring_enqueue(r, PTR(1));
	assert(ring_dequeue(r) == PTR(1));

	assert(ring_empty(r));
	assert(!ring_full(r));

	for (i = 0; i < SIZE - 1; i++)
		assert(ring_enqueue(r, PTR(i)));

	assert(!ring_enqueue(r, PTR(i)));
	assert(!ring_empty(r));
	assert(ring_full(r));

	ring_destroy(r);

	pthread_create(&tid, NULL, consumer, NULL);

	while (1) {
		int *i = (int *)malloc(sizeof(int));
		*i = 100;
		ring_enqueue(r, (void *)i);
	}

	//never core

}




#include "event.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>

static inline uint64_t get_current_msec()
{
	struct timespec ts;

	clock_gettime(CLOCK_MONOTONIC, &ts);

	return ts.tv_sec * 1000 + ts.tv_nsec / 1000 / 1000;
}

#define SIZE 10

ev_timer_t timer_array[SIZE];
ev_context_t *ev;

void timer_callback(ev_timer_t * timer)
{
	long i = (long)(timer->data);

	if (i == 0)
		usleep(5000);

	assert(timer == &timer_array[i]);

	printf("current time = %ld, timer.msec = %ld, index = %ld\n",
	       get_current_msec(), timer->msec, i);

	ev_cancel_timer(ev, &timer_array[i + 1]);

	if (i + 1 == SIZE - 1)
		ev->stopped = 1;
}

int main()
{
	long i;
	ev = ev_create_context(SIZE);

	for (i = 0; i < SIZE; i++) {
		timer_array[i].data = (void *)(i);
		ev_init_timer(&timer_array[i], (i + 1), timer_callback);
	}

	printf("current time = %ld.\n", get_current_msec());

	for (i = 0; i < SIZE; i++) {
		ev_start_timer(ev, &timer_array[i]);
	}

	ev_run(ev);

	ev_destory_context(ev);
}
