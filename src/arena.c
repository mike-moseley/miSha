#include "alloc_error.h"
#include "sys/mman.h"
#include "arena.h"
#include "alloc_common.h"

AllocError arenaCreate(size_t capacity, arena_t **out) {
	arena_t *arena;

	if(out == NULL) return ALLOC_ERROR_NULL;
	if(capacity == 0) return ALLOC_ERROR_SIZE;

	/*                            | Header len  | + | buf len| */
	arena = (arena_t *)mmap(NULL, sizeof(arena_t) + capacity, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if(arena == MAP_FAILED) return ALLOC_ERROR_OOM;

	/* buf is the data in the arena, must offset by the size of the header*/
	arena->buf = (char *)arena + sizeof(arena_t);
	arena->offset = 0;
	arena->capacity = capacity;
	*out = arena;

	return ALLOC_OK;
}

/*
 * We are working on pointers, so we need a pointer(void **out)
 * to the pointer(void *out) we want to work with
 * otherwise we would be working on a copy of the pointer and not the pointer
 * itself...
 * **out (* operator) is the data, *out is the pointer to the data(what we are manipulating)
 * and out is the pointer to the pointer we are working with.
*/
AllocError arenaAlloc(arena_t *arena, size_t size, void **out) {
	if(arena == NULL) return ALLOC_ERROR_NULL;
	if(out == NULL) return ALLOC_ERROR_NULL;
	if(size == 0) return ALLOC_ERROR_SIZE;
	if(ALIGN(size) > arena->capacity - arena->offset) return ALLOC_ERROR_SIZE;

	*out = arena->buf + arena->offset;
	arena->offset += ALIGN(size);
	
	return ALLOC_OK;
}

void arenaReset(arena_t *arena) {
	if(arena == NULL) return;
	arena->offset = 0;
}

void arenaDestroy(arena_t *arena) {
	if(arena==NULL) return;
	/*            | Header len  | + |  buf len     | */
	munmap(arena, sizeof(arena_t) + arena->capacity);
}
