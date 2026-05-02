#include "shell/executor.h"
#include "shell/input.h"
#include "shell/parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int execute(command_t *cmd) {
	pid_t pid;
	int status;

	if(cmd == NULL) return -1;

	if(cmd->next == NULL) {
		pid = fork();
		switch(pid) {
		case -1:
			perror("\nfork in non-pipe execute in executor.c\n");
			exit(EXIT_FAILURE);
		case 0:
			restoreTerminal();
			execvp(cmd->argv[0], cmd->argv);
			/* Only reachable if execvp fails */
			perror("\nexecvp in non-pipe execute in executor.c\n");
			exit(EXIT_FAILURE);
		default:
			waitpid(pid, &status, 0);
			enableRawMode();
			return status;
		}
	} else {
		int pipefd[2];
		int lhs_pid;
		int rhs_pid;
		if(pipe(pipefd) == -1) return -1;
		lhs_pid = fork();
		switch(lhs_pid) {
		case -1:
			perror("\nfork in pipe execute");
			exit(EXIT_FAILURE);
		case 0:
			restoreTerminal();
			dup2(pipefd[1], STDOUT_FILENO);
			close(pipefd[0]);
			close(pipefd[1]);
			execvp(cmd->argv[0], cmd->argv);
			/* Only reachable if execvp fails */
			perror("\nexecvp in pipe execute in executor.c\n");
			exit(EXIT_FAILURE);
		default:
			rhs_pid = fork();
			switch(rhs_pid) {
			case -1:
				perror("\nfork in pipe execute");
				exit(EXIT_FAILURE);
			case 0:
				restoreTerminal();
				dup2(pipefd[0], STDIN_FILENO);
				close(pipefd[0]);
				close(pipefd[1]);
				execute(cmd->next);
				exit(EXIT_SUCCESS);
			default:
				close(pipefd[0]);
				close(pipefd[1]);
				waitpid(lhs_pid, &status, 0);
				waitpid(rhs_pid, &status, 0);
				enableRawMode();
				return status;
			}
		}
	}
}
