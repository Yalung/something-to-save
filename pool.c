/* 
 *  Only for linux x86_64 and gcc!!!
 *  
 *  yalung929@gmail.com 
 *  2014.3 F1 Bantian Shenzhen China 
 */


#include "common.h"
#include "pool.h"
#include <assert.h>

static inline void *objmem_to_obj(void *objmem)
{
	return objmem + sizeof(pool_obj_head_t);
}

static inline void *obj_to_objmem(void *obj)
{
	return obj - sizeof(pool_obj_head_t);
}

static inline void *idx_to_objmem(pool_t * p, pool_size_t idx)
{
	return p->obj + idx * (p->objmemsize);
}

static inline pool_size_t objmem_to_idx(pool_t * p, void *obj)
{
	return (obj - p->obj) / (p->objmemsize);
}

pool_t *pool_create(pool_obj_size_t objsize, pool_size_t poolsize)
{

	pool_size_t i;
	pool_obj_head_t *head;

	pool_t *p;

	size_t manage_mem_size = (sizeof(pool_t));
	size_t free_list_mem_size = (poolsize * sizeof(pool_size_t));
	size_t obj_mem_size = (objsize + sizeof(pool_obj_head_t)) * poolsize;

	p = calloc(manage_mem_size + free_list_mem_size + obj_mem_size, 1);

	if (!p)
		return NULL;

	p->num = poolsize;
	p->objsize = objsize;
	p->objmemsize = obj_mem_size / poolsize;
	p->freeobj = (void *)p->buffer;
	p->obj = p->buffer + free_list_mem_size;

	/* init free list */
	p->free_idx = 0;
	for (i = 0; i < p->num; i++) {
		p->freeobj[i] = i + 1;
	}

	/* init obj head */
	for (i = 0; i < p->num; i++) {
		head = (pool_obj_head_t *) (idx_to_objmem(p, i));
		head->p = p;
	}
	return p;
}

void *pool_alloc_obj(pool_t * p)
{
	pool_size_t idx;
	void *obj;

	if (unlikely(p->free_idx == p->num)) {
		return NULL;	/* empty */
	}

	idx = p->free_idx;
	p->free_idx = p->freeobj[idx];

	obj = idx_to_objmem(p, idx);

	return objmem_to_obj(obj);
}

void pool_free_obj(pool_t * p, void *obj)
{

	pool_size_t idx;
	pool_obj_head_t *head;

	head = (pool_obj_head_t *) obj_to_objmem(obj);

	assert(head->p == p);

	idx = objmem_to_idx(p, obj);

	/* insert to list head, hot cache */
	p->freeobj[idx] = p->free_idx;
	p->free_idx = idx;
}

void pool_destory(pool_t * p)
{
	free(p);
}
