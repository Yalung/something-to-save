.PHONY: all 

INCLUDE := ../include/ -I../lib/
CCFLAGS := -std=gnu99 -g -O2 -Wall -Wextra -I$(INCLUDE) -pthread -lrt 

all: test_htable test_timer test_pool test_lcserver test_ring run

clean:
	rm -f test_ring test_timer test_htable test_pool test_lcserver *.o *~ *.gch

test_htable: test_htable.c ../lib/htable.c 
	gcc $(CCFLAGS) $^ -o $@ 

test_pool: test_pool.c ../lib/pool.c
	gcc $(CCFLAGS) $^ -o $@

test_lcserver: test_lcserver.c ../lib/lcserver.c ../lib/network.c ../lib/pool.c ../lib/event.c
	gcc $(CCFLAGS) $^ -o $@

test_ring: test_ring.c 
	gcc $(CCFLAGS) $^ -o $@

test_timer: test_timer.c ../lib/event.c
	gcc $(CCFLAGS) $^ -o $@

run: test_htable test_pool test_timer 
	./test_htable;./test_pool;./test_timer;
