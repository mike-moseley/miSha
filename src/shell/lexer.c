#include "shell/lexer.h"
#include "shell/consts.h"
#include "shell/env.h"
#include "shell/parser.h"
#include "vendor/alloc/alloc_error.h"
#include "vendor/alloc/arena.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

typedef enum { NORMAL, IN_DOUBLE_QUOTE, IN_SINGLE_QUOTE } lexer_state_t;

void lexer(char *input, command_t *cmd, arena_t *arena) {
	char *token, *str;
	lexer_state_t state;
	char *token_ptr;
	char *token_start;
	AllocError err;
	int argc;
	int i;

	if(cmd == NULL) return;

	err = arenaAlloc(arena, BUF_SIZE, (void **)&token_ptr);

	token_start = token_ptr;
	i = 0;
	argc = 0;
	state = NORMAL;
	while(input[i] != '\0') {
		switch(input[i]) {
		case '\t':
		case ' ': {
			if((state == NORMAL) && (token_ptr != token_start)) {
				*token_ptr = '\0';
				cmd->argv[argc] = token_start;
				argc++;
				err = arenaAlloc(arena, BUF_SIZE, (void **)&token_ptr);
				token_start = token_ptr;
			}
			break;
		}

		case '"': {
			if(state == NORMAL) {
				state = IN_DOUBLE_QUOTE;
			} else if(state == IN_DOUBLE_QUOTE) {
				state = NORMAL;
			} else {
				*token_ptr = input[i];
				token_ptr++;
			}
			break;
		}
		case '\'': {
			if(state == NORMAL) {
				state = IN_SINGLE_QUOTE;
			} else if(state == IN_SINGLE_QUOTE) {
				state = NORMAL;
			} else {
				*token_ptr = input[i];
				token_ptr++;
			}
			break;
		}
		case '$': {
			char tmp[BUF_SIZE];
			int j;
			int t;
			j = i + 1;
			t = 0;
			while(isalnum(input[j])) {
				tmp[t] = input[j];
				j++;
				t++;
			}
			tmp[t] = '\0';
			token = getEnv(tmp);
			if(token != NULL) {
				strcpy(token_ptr, token);
				token_ptr = token_ptr + strlen(token);
			}
			i = j - 1;
			break;
		}
		case '>': {
			char tmp[BUF_SIZE];
			if(input[i + 1] == '>') cmd->append = 1;
			read_token(input, &i, tmp);
			if(tmp[0] == '\0') {
				break;
			}
			err = arenaAlloc(arena, BUF_SIZE, (void **)&token_ptr);
			strcpy(token_ptr, tmp);
			cmd->redirect_out = token_ptr;
			cmd->argv[argc] = NULL;
			err = arenaAlloc(arena, BUF_SIZE, (void **)&token_ptr);
			token_start = token_ptr;
			break;
		}
		case '<': {
			break;
		}
		default:
			*token_ptr = input[i];
			token_ptr++;
		}
		i++;
	}
}

void read_token(char *input, int *i, char *buf) {
	int t;
	t = 0;
	while(isspace(input[*i]))
		i++;
	while(!isspace(input[*i])) {
		buf[t] = input[*i];
		(*i)++;
		t++;
	}
	buf[t] = '\0';
}
