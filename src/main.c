#include <stdlib.h>
#include <termios.h>
#include "history.h"
#include "input.h"
#include "lexer.h"
#include "executor.h"
#include "builtins.h"
#include "consts.h"
#include "env.h"

int main(void) {
	char buf[BUF_SIZE];
	char *argv[MAX_ARGS];
	BuiltinCode builtin_code;

	initEnv();
	initHistory();

	enableRawMode();
	atexit(restoreTerminal);

	while(1) {
		if(readline_raw(buf, sizeof(buf)) == -1) break;

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
