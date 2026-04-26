#ifndef BUILTINS_H
#define BUILTINS_H

enum builtin_codes {
	BUILTIN_OK = 0,
	NOT_BUILTIN = 1,
	BUILTIN_FAILED = -1
};

int handle_builtins(char **argv);

#endif
