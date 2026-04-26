#include "builtins.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int handle_builtins(char **argv) {
	char *command;
	command = argv[0];

	if(strcmp("exit", argv[0]) == 0) {
		exit(EXIT_SUCCESS);

	} else if (strcmp("cd", command) == 0) {
		if (argv[1] == NULL || *argv[1] == '\0') {
			puts("Please provide a directory");
			return BUILTIN_FAILED;
		}
		if (chdir(argv[1]) == -1) {
			perror("chdir in cd in handle_builtins failed");
			return BUILTIN_FAILED;
		}
		return BUILTIN_OK;

	} else {
		return NOT_BUILTIN;
	}

}
