#ifndef PARSER_H
#define PARSER_H

#include "vendor/alloc/arena.h"
#include "vendor/alloc/pool.h"

typedef struct Command{
	struct Command *next;
	char **argv;
	char *redirect_in;
	char *redirect_out;
	char *redirect_err;
	int append;
} command_t;

command_t *parseCommands(char *input, pool_t *pool, arena_t *arena);

#endif
