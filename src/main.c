#include <stdlib.h>
#include <termios.h>
#include "arena.h"
#include "history.h"
#include "input.h"
#include "executor.h"
#include "parser.h"
#include "builtins.h"
#include "consts.h"
#include "env.h"
#include "pool.h"

arena_t *parser_arena;
pool_t *parser_pool;

void cleanupArenas(void) {
	arenaDestroy(parser_arena);
}

void cleanupPools(void) {
	poolDestroy(parser_pool);
}

int main(void) {
	char buf[BUF_SIZE];
	BuiltinCode builtin_code;
	command_t *cmd;

	arenaCreate(MAX_ARGS * sizeof(char *) * PIPE_DEPTH, &parser_arena);
	atexit(cleanupArenas);
	poolCreate(MAX_ARGS, sizeof(command_t), &parser_pool);
	atexit(cleanupPools);

	initEnv();
	initHistory();

	enableRawMode();
	atexit(restoreTerminal);

	while(1) {
		if(readline_raw(buf, BUF_SIZE) == -1) break;

		if(buf[0] == '\0') continue;

		pushHistory(buf);
		cmd = parseCommands(buf, parser_pool, parser_arena);
		if (cmd == NULL) continue;
		builtin_code = handle_builtins(cmd->argv);
		if (builtin_code == NOT_BUILTIN) {
			execute(cmd->argv);
		}
		arenaReset(parser_arena);
		poolReset(parser_pool);
	}

	return 0;
}
