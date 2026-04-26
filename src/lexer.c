#include "lexer.h"
#include <string.h>

void lexer(char *input, char **argv) {
	char *token, *str;
	int i;
	for(i=0, str=input; ;i++, str=NULL) {
		token = strtok(str, " \t");
		if (token == NULL) {
			argv[i] = NULL;
			break;
		}
		argv[i] = token;
	}
}
