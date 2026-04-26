#ifndef BUILTINS_H
#define BUILTINS_H

typedef enum {
	BUILTIN_OK = 0,
	NOT_BUILTIN = 1,
	BUILTIN_FAILED = -1
} BuiltinCode;

typedef enum{
	BUILTIN_NOT_FOUND = -1,
	BUILTIN_EXIT,
	BUILTIN_CD,
	BUILTIN_PWD
} BuiltinCommmands;

BuiltinCode handle_builtins(char **argv);

#endif
