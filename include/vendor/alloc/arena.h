#ifndef ARENA_H
#define ARENA_H

#include "alloc_error.h"
#include <stddef.h>

typedef struct {
	char *buf;
	size_t offset;
	size_t capacity;
} arena_t;

AllocError arenaCreate(size_t capacity, arena_t **out);
AllocError arenaAlloc(arena_t *arena, size_t size, void **out);
void arenaReset(arena_t *arena);
void arenaDestroy(arena_t *arena);

#endif
