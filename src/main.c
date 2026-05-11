#include "shell/builtins.h"
#include "shell/consts.h"
#include "shell/env.h"
#include "shell/executor.h"
#include "shell/history.h"
#include "shell/input.h"
#include "shell/parser.h"
#include "vendor/alloc/arena.h"
#include "vendor/alloc/pool.h"
#include "vendor/data-structures/slice.h"
#include <signal.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <termios.h>

arena_t *parser_arena;
pool_t *parser_pool;
slice_t *input;

void cleanupArenas(void) { arenaDestroy(parser_arena); }

void cleanupPools(void) { poolDestroy(parser_pool); }

void sigchld_handler(int sig) {
	(void)sig;
	while(waitpid(-1, NULL, WNOHANG) > 0);
}

int main(void) {
	BuiltinCode builtin_code;
	command_t *cmd;

	signal(SIGINT, SIG_IGN);
	signal(SIGCHLD, sigchld_handler);
	arenaCreate((MAX_ARGS * sizeof(char *)
			 + (MAX_ARGS * BUF_SIZE))
			 * PIPE_DEPTH, &parser_arena);
	
	atexit(cleanupArenas);
	poolCreate(MAX_ARGS, sizeof(command_t), &parser_pool);
	atexit(cleanupPools);

	initTerminal();
	initEnv();
	initHistory();

	enableRawMode();
	atexit(restoreTerminal);

	while(1) {
		input = createSlice(sizeof(char), 0);
		if(readline_raw(input) == -1) break;

		if(((char *)input->arr)[0] == '\0') continue;

		pushHistory((char *)input->arr);
		cmd = parseCommands((char *)input->arr, parser_pool, parser_arena);
		if(cmd == NULL) continue;
		builtin_code = handle_builtins(cmd->argv);
		if(builtin_code == NOT_BUILTIN) {
			execute(cmd);
		}
		arenaReset(parser_arena);
		poolReset(parser_pool);
		freeSlice(input, NULL);
	}

	return 0;
}
