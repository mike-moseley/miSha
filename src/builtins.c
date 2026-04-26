#include "builtins.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

BuiltinCommmands get_builtin_command(char *command) {
	if (strcmp("cd", command) == 0) {
		return BUILTIN_CD;
	} else if (strcmp("pwd", command) == 0) {
		return BUILTIN_PWD;
	} else if (strcmp("exit", command) == 0) {
		return BUILTIN_EXIT;
	} else {
		return BUILTIN_NOT_FOUND;
	}
}

BuiltinCode builtin_cd(char **argv) {
	if (argv[1] == NULL || *argv[1] == '\0') {
		puts("Please provide a directory");
		return BUILTIN_FAILED;
	}
	if (chdir(argv[1]) == -1) {
		perror("chdir in cd in handle_builtins failed");
		return BUILTIN_FAILED;
	}
	return BUILTIN_OK;
}

BuiltinCode handle_builtins(char **argv) {
	char *command;
	BuiltinCommmands builtin;
	command = argv[0];
	builtin = get_builtin_command(command);

	switch(builtin) {
		case BUILTIN_EXIT:
			exit(EXIT_SUCCESS);
		case BUILTIN_CD:
			return builtin_cd(argv);
		case BUILTIN_PWD:
			return BUILTIN_OK;
		case BUILTIN_NOT_FOUND:
			return NOT_BUILTIN;
		default:
			return BUILTIN_FAILED;
	}
}
