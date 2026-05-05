#ifndef LEXER_H
#define LEXER_H

#include "shell/parser.h"
#include "vendor/alloc/arena.h"
void lexer(char *input, command_t *cmd, arena_t *arena);
void read_token(char *input, int *i, char *buf);

#endif
