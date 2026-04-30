#include "parser.h"
#include "alloc_error.h"
#include "arena.h"
#include "consts.h"
#include "lexer.h"
#include <string.h>

command_t *parseCommands(char *input, pool_t *pool, arena_t *arena) {
	char *delimit;
	void *ptr;
	command_t *node;
	AllocError alloc_err;

	alloc_err = poolAlloc(pool,&ptr);
	if (alloc_err != ALLOC_OK) {
		return NULL;
	}
	node = (command_t *)ptr;

	alloc_err = arenaAlloc(arena, MAX_ARGS * sizeof(char *), (void **)&node->argv);

	delimit = strchr(input,'|');
	if(delimit == NULL) {
		lexer(input, node->argv);
		node->next = NULL;
	} else {
		*delimit = '\0';
		lexer(input, node->argv);
		node->next = parseCommands(delimit+1, pool, arena);
	}
	return node;
}
