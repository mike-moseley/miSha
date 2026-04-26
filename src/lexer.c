#include "lexer.h"
#include <stdlib.h>
#include <string.h>
#include "env.h"

void lexer(char *input, char **argv) {
	char *token, *str;
	int i;
	for(i=0, str=input; ;i++, str=NULL) {
		token = strtok(str, " \t");
		if (token == NULL) {
			argv[i] = NULL;
			break;
		}
		if (token[0] == '$') {
			token = getEnv(token+1);
			if(token == NULL) {
				token = "";
			}
		}
		argv[i] = token;
	}
}
