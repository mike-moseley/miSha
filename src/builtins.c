#define _POSIX_C_SOURCE 200112L
#include "builtins.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "consts.h"

BuiltinCommmands get_builtin_command(char *command) {
	if (strcmp("cd", command) == 0) {
		return BUILTIN_CD;
	} else if (strcmp("pwd", command) == 0) {
		return BUILTIN_PWD;
	} else if (strcmp("export", command) == 0) {
		return BUILTIN_EXPORT;
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
		perror("chdir in builtin_cd in builtins.c failed");
		return BUILTIN_FAILED;
	}
	return BUILTIN_OK;
}

BuiltinCode builtin_pwd(void) {
	char buf[BUF_SIZE];	

	if(getcwd(buf, BUF_SIZE) == NULL) {
		return BUILTIN_FAILED;
	}

	puts(buf);
	return BUILTIN_OK;
}

BuiltinCode builtin_export(char **argv) {
	char *delimit_ptr;
	char *key;
	char *value;
	if (argv[1] == NULL || *argv[1] == '\0') {
		puts("Please provide environment variable in this syntax `export KEY=value`");
		return BUILTIN_FAILED;
	}
	delimit_ptr = strchr(argv[1],'=');
	if(delimit_ptr == NULL) {
		fprintf(stderr, "strchr in builtin_export in builtins.c: Possibly malformed input");
		return BUILTIN_FAILED;
	}
	*delimit_ptr = '\0';
	key = argv[1];
	value = delimit_ptr + 1;
	if(setenv(key, value, 0) == -1) {
		perror("setenv in builtin_export in builtins.c");
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
			return builtin_pwd();
		case BUILTIN_EXPORT:
			return builtin_export(argv);
		case BUILTIN_NOT_FOUND:
			return NOT_BUILTIN;
		default:
			return BUILTIN_FAILED;
	}
}
