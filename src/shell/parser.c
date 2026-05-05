#include "shell/parser.h"
#include "vendor/alloc/alloc_error.h"
#include "vendor/alloc/arena.h"
#include "shell/consts.h"
#include "shell/lexer.h"
#include <stdio.h>
#include <string.h>

command_t *parseCommands(char *input, pool_t *pool, arena_t *arena) {
	char *delimit;
	void *ptr;
	command_t *cmd;
	AllocError alloc_err;

	alloc_err = poolAlloc(pool,&ptr);
	if (alloc_err != ALLOC_OK) {
		perror("Pool allocation error in parseCommands in parser.c");
		return NULL;
	}

	cmd = (command_t *)ptr;
	memset(cmd, 0, sizeof(command_t));

	alloc_err = arenaAlloc(arena, MAX_ARGS * sizeof(char *), (void **)&cmd->argv);
	if (alloc_err != ALLOC_OK) {
		perror("Arena allocation error in parseCommands in parser.c");
		return NULL;
	}

	delimit = strchr(input,'|');
	if(delimit == NULL) {
		lexer(input, cmd, arena);
		cmd->next = NULL;
	} else {
		*delimit = '\0';
		lexer(input, cmd, arena);
		cmd->next = parseCommands(delimit+1, pool, arena);
	}
	return cmd;
}
