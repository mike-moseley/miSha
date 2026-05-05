#include "shell/consts.h"
#include "shell/env.h"
#include "shell/lexer.h"
#include "shell/parser.h"
#include "unity.h"
#include "unity_internals.h"
#include "vendor/alloc/alloc_error.h"
#include "vendor/alloc/arena.h"
#include "vendor/alloc/pool.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

arena_t *arena;
pool_t *pool;
command_t *cmd;
AllocError err;

void setUp(void) {
	err = arenaCreate(MAX_ARGS * BUF_SIZE, &arena);
	if(err != ALLOC_OK) {
		perror("Error allocating arena in test_lexer");
		return;
	}
	err = poolCreate(MAX_ARGS, sizeof(command_t), &pool);
	if(err != ALLOC_OK) {
		perror("Error allocating pool in test_lexer");
		return;
	}
	err = poolAlloc(pool, (void *)&cmd);
	memset(cmd, 0, sizeof(command_t));
	err = arenaAlloc(arena, MAX_ARGS * sizeof(char *), (void **)&cmd->argv);
	if(err != ALLOC_OK) {
		perror("Error allocating arena in test_lexer");
		return;
	}
}
void tearDown(void) {
	arenaDestroy(arena);
	poolDestroy(pool);
}

void test_single_command(void) {
	char input[] = "ls -la";
	lexer(input, cmd, arena);
	TEST_ASSERT_EQUAL_STRING("ls", cmd->argv[0]);
	TEST_ASSERT_EQUAL_STRING("-la", cmd->argv[1]);
	TEST_ASSERT_NULL(cmd->argv[2]);
}

void test_num_tokens(void) {
	char input[] = "echo hello world";
	lexer(input, cmd, arena);
	TEST_ASSERT_NOT_NULL(cmd->argv[0]);
	TEST_ASSERT_NOT_NULL(cmd->argv[1]);
	TEST_ASSERT_NOT_NULL(cmd->argv[2]);
	TEST_ASSERT_NULL(cmd->argv[3]);
}

void test_empty(void) {
	char input[] = "";
	lexer(input, cmd, arena);
	TEST_ASSERT_NULL(cmd->argv[0]);
}

void test_whitespace(void) {
	char input[] = " ls\t-la     ";
	lexer(input, cmd, arena);
	TEST_ASSERT_EQUAL_STRING("ls", cmd->argv[0]);
	TEST_ASSERT_EQUAL_STRING("-la", cmd->argv[1]);
	TEST_ASSERT_NULL(cmd->argv[2]);
}

void test_env_var_expansion(void) {
	char input[] = "echo $HOME";
	lexer(input, cmd, arena);
	TEST_ASSERT_EQUAL_STRING("echo", cmd->argv[0]);
	TEST_ASSERT_EQUAL_STRING(getenv("HOME"), cmd->argv[1]);
	TEST_ASSERT_NULL(cmd->argv[2]);
}

void test_undefined_env_var(void) {
	char input[] = "echo $BRAND_NEW_ENV_VAR";
	lexer(input, cmd, arena);
	TEST_ASSERT_EQUAL_STRING("echo", cmd->argv[0]);
	TEST_ASSERT_EQUAL_STRING(NULL, cmd->argv[1]);
	TEST_ASSERT_NULL(cmd->argv[2]);
}

int main(void) {
	initEnv();
	UNITY_BEGIN();
	RUN_TEST(test_single_command);
	RUN_TEST(test_num_tokens);
	RUN_TEST(test_empty);
	RUN_TEST(test_whitespace);
	RUN_TEST(test_env_var_expansion);
	RUN_TEST(test_undefined_env_var);
	return UNITY_END();
}
