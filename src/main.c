#include <stdlib.h>
#include <termios.h>
#include "alloc_error.h"
#include "arena.h"
#include "history.h"
#include "input.h"
#include "lexer.h"
#include "executor.h"
#include "builtins.h"
#include "consts.h"
#include "env.h"

arena_t *command_mem;

void cleanupArena(void) {
	arenaDestroy(command_mem);
}

int main(void) {
	char *buf;
	char **argv;
	BuiltinCode builtin_code;
	AllocError alloc_err;

	alloc_err = arenaCreate(BUF_SIZE * MAX_ARGS, &command_mem);
	atexit(cleanupArena);

	if (alloc_err != ALLOC_OK) {
		return -1;
	}

	initEnv();
	initHistory();

	enableRawMode();
	atexit(restoreTerminal);

	while(1) {
		arenaReset(command_mem);
		arenaAlloc(command_mem, BUF_SIZE, (void *)&buf);
		arenaAlloc(command_mem, MAX_ARGS * sizeof(char *), (void **)&argv);

		if(readline_raw(buf, BUF_SIZE) == -1) break;

		if(buf[0] == '\0') continue;

		pushHistory(buf);
		lexer(buf, argv);
		builtin_code = handle_builtins(argv);
		if (builtin_code == NOT_BUILTIN) {
			execute(argv);
		}
	}

	return 0;
}
