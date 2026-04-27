#include "executor.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int execute(char **argv) {
	pid_t pid;
	int status;

	pid = fork();

	switch(pid) {
		case -1:
			perror("\nfork in execute in executor.c\n");
			exit(EXIT_FAILURE);
		case 0:
			execvp(argv[0], argv);
			perror("\nexecvp in execute in executor.c\n");
			exit(EXIT_FAILURE);
		default:
			waitpid(pid, &status, 0);
			return status;
	}
}
