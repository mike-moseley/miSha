#include "vendor/alloc/pool.h"
#include "vendor/alloc/alloc_error.h"
#include "vendor/alloc/alloc_common.h"
#include <sys/mman.h>

typedef struct chunk_t chunk_t;
struct chunk_t {
	chunk_t *next;
};

static AllocError link_chunks(pool_t *pool, chunk_t *free_list) {
	size_t i;
	chunk_t *tmp_chunk;
	chunk_t *chunk = free_list;

	if(pool == NULL) return ALLOC_ERROR_NULL; 

	/* Last iteration chunk->next must be null; handle after loop. */
	for(i=0; i<pool->count-1; i++) {
		tmp_chunk = (chunk_t *)((char *)chunk + pool->chunk_size);
		chunk->next = tmp_chunk;
		chunk = tmp_chunk;
	}
	chunk->next = NULL;

	return ALLOC_OK;
}

AllocError poolCreate(size_t count, size_t chunk_size, pool_t **out) {
	pool_t *pool;
	size_t pool_size;
	AllocError err;

	if(count == 0) return ALLOC_ERROR_SIZE;
	if(chunk_size == 0) return ALLOC_ERROR_SIZE;
	if(out == NULL) return ALLOC_ERROR_NULL;

	chunk_size = ALIGN(chunk_size);
	pool_size = chunk_size * count;

	pool = (pool_t *)mmap(NULL, sizeof(pool_t) + pool_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if(pool == MAP_FAILED) return ALLOC_ERROR_OOM;
	
	pool->buf = (char *)pool + sizeof(pool_t);
	pool->chunk_size = chunk_size;
	pool->count = count;
	pool->free_list = (chunk_t *)pool->buf;
	err = link_chunks(pool, pool->free_list);
	if (err != ALLOC_OK) {
		return err;
	}
	*out = pool;
	
	return ALLOC_OK;
}

AllocError poolAlloc(pool_t *pool, void **out) {
	chunk_t *head;
	if(pool == NULL) return ALLOC_ERROR_NULL;
	if(out == NULL) return ALLOC_ERROR_NULL;
	if(pool->free_list == NULL) return ALLOC_ERROR_OOM;

	head = (chunk_t *)pool->free_list;
	if(head == NULL) return ALLOC_ERROR_OOM;
	pool->free_list = head->next;
	*out = head;
	return ALLOC_OK;
}

void poolFree(pool_t *pool, void *chunk) {
	if(pool == NULL) return ;
	if(chunk == NULL) return ;

	((chunk_t *)chunk)->next = (chunk_t *)pool->free_list;
	pool->free_list = chunk;
}

void poolReset(pool_t *pool) {
	pool->free_list = (chunk_t *)pool->buf;
	link_chunks(pool, pool->free_list);
}

void poolDestroy(pool_t *pool) {
	if(pool == NULL) return;
	munmap(pool, sizeof(pool_t) + pool->chunk_size * pool->count);
}
