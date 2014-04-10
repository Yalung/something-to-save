/* 
 *  Only for linux x86_64 and gcc!!!
 *  
 *  yalung929@gmail.com 
 *  2014.3 F1 Bantian Shenzhen China 
 */


#ifndef POOL_HHH
#define POOL_HHH

#include "common.h"

/* simple and fast obj pool without multi-thread support */

typedef uint32_t pool_size_t;
typedef uint32_t pool_obj_size_t;

typedef struct pool {

	pool_obj_size_t objsize;
	pool_obj_size_t objmemsize;

	pool_size_t num;

	pool_size_t free_idx;	/* free list implemented by array */
	pool_size_t *freeobj;	/* free list array */

	void *obj;		/* obj array */
	char buffer[0];
} pool_t;

typedef struct pool_obj_head {
	pool_t *p;

} pool_obj_head_t;

pool_t *pool_create(pool_obj_size_t objsize, pool_size_t poolsize);

void *pool_alloc_obj(pool_t * p);

void pool_free_obj(pool_t * p, void *obj);

void pool_destory(pool_t * p);

#endif
