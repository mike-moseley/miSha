#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "executor.h"
#include "builtins.h"

#define BUF_SIZE 1024
#define MAX_ARGS 16

int main(void) {
	char buf[BUF_SIZE];
	char *argv[MAX_ARGS];
	enum builtin_codes builtin_code;

	while(1) {
		printf("$ ");
		fflush(stdout);

		if(fgets(buf, sizeof(buf), stdin) == NULL) break;

		buf[strcspn(buf, "\n")] = '\0';

		if(buf[0] == '\0') continue;

		lexer(buf, argv);
		builtin_code = handle_builtins(argv);
		if (builtin_code == NOT_BUILTIN) {
			execute(argv);
		}
	}

	return 0;
}
