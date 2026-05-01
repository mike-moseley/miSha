#ifndef PARSER_H
#define PARSER_H

#include "arena.h"
#include "pool.h"

typedef struct Command{
	struct Command *next;
	char **argv;
} command_t;

command_t *parseCommands(char *input, pool_t *pool, arena_t *arena);

#endif
