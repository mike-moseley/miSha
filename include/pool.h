#ifndef POOL_H
#define POOL_H

#include "alloc_error.h"
#include <stddef.h>

typedef struct {
	char *buf;
	size_t count;
	size_t chunk_size;
	void *free_list;
} pool_t;

AllocError poolCreate(size_t count, size_t chunk_size, pool_t **out);
AllocError poolAlloc(pool_t *pool, void **out);
void poolFree(pool_t *pool, void *chunk);
void poolReset(pool_t *pool);
void poolDestroy(pool_t *pool);

#endif
