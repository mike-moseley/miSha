#include <stdlib.h>
#include <string.h>
#include "shell/lexer.h"
#include "shell/env.h"
#include "shell/parser.h"

void lexer(char *input, command_t *cmd) {
	char *token, *str;
	int i;

	if (cmd == NULL) return;

	for(i=0, str=input; ;i++, str=NULL) {
		token = strtok(str, " \t");
		if (token == NULL) {
			cmd->argv[i] = NULL;
			break;
		}
		if (token[0] == '$') {
			token = getEnv(token+1);
			if(token == NULL) {
				token = "";
			}
		}
		if(token[0] == '2') {
			if(token[1] == '>') {
				if(token[2] == '>'){
					cmd->append = 1;
				}
				cmd->redirect_err = strtok(NULL, " \t");
				cmd->argv[i] = NULL;
				return;
			}
		}
		if (token[0] == '>') {
			if(token[1] == '>') {
				cmd->append = 1;
			}
			cmd->redirect_out = strtok(NULL, " \t");
			cmd->argv[i] = NULL;
			return;
		}
		if (token[0] == '<') {
			cmd->redirect_in = strtok(NULL, " \t");
			cmd->argv[i] = NULL;
			return;
		}
		cmd->argv[i] = token;
	}
}
