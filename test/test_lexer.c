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

void test_double_quotes_dont_split_space(void) {
	char input[] = "echo \"hello world\"";
	lexer(input, cmd, arena);
	TEST_ASSERT_EQUAL_STRING("echo", cmd->argv[0]);
	TEST_ASSERT_EQUAL_STRING("hello world", cmd->argv[1]);
	TEST_ASSERT_NULL(cmd->argv[2]);
}

void test_single_quotes_dont_split_space(void) {
	char input[] = "echo \'hello world\'";
	lexer(input, cmd, arena);
	TEST_ASSERT_EQUAL_STRING("echo", cmd->argv[0]);
	TEST_ASSERT_EQUAL_STRING("hello world", cmd->argv[1]);
	TEST_ASSERT_NULL(cmd->argv[2]);
}

void test_double_quotes_allows_var_expansion(void) {
	char input[] = "echo \"$HOME\"";
	lexer(input, cmd, arena);
	TEST_ASSERT_EQUAL_STRING("echo", cmd->argv[0]);
	TEST_ASSERT_EQUAL_STRING("/home/mike", cmd->argv[1]);
	TEST_ASSERT_NULL(cmd->argv[2]);
}

void test_single_quotes_suppress_var_expansion(void) {
	char input[] = "echo \'$HOME\'";
	lexer(input, cmd, arena);
	TEST_ASSERT_EQUAL_STRING("echo", cmd->argv[0]);
	TEST_ASSERT_EQUAL_STRING("$HOME", cmd->argv[1]);
	TEST_ASSERT_NULL(cmd->argv[2]);
}

void test_unquoted_quoted_mixes(void) {
	char input[] = "echo ding\"dong ding\"";
	lexer(input, cmd, arena);
	TEST_ASSERT_EQUAL_STRING("echo", cmd->argv[0]);
	TEST_ASSERT_EQUAL_STRING("dingdong ding", cmd->argv[1]);
	TEST_ASSERT_NULL(cmd->argv[2]);
}

void test_redirect_out(void) {
	char input[] = "echo hello > out.txt";
	lexer(input, cmd, arena);
	TEST_ASSERT_EQUAL_STRING("out.txt", cmd->redirect_out);
	TEST_ASSERT_EQUAL_INT(cmd->append, 0);
}

void test_redirect_out_append(void) {
	char input[] = "echo hello >> out.txt";
	lexer(input, cmd, arena);
	TEST_ASSERT_EQUAL_STRING("out.txt", cmd->redirect_out);
	TEST_ASSERT_EQUAL_INT(cmd->append, 1);
}

void test_redirect_in(void) {
	char input[] = "cat < in.txt";
	lexer(input, cmd, arena);
	TEST_ASSERT_EQUAL_STRING("in.txt", cmd->redirect_in);
}

void test_redirect_err(void) {
	char input[] = "false 2> err.txt";
	lexer(input, cmd, arena);
	TEST_ASSERT_EQUAL_STRING("err.txt", cmd->redirect_err);
	TEST_ASSERT_EQUAL_INT(cmd->append_err, 0);
}

void test_redirect_err_append(void) {
	char input[] = "false 2>> err.txt";
	lexer(input, cmd, arena);
	TEST_ASSERT_EQUAL_STRING("err.txt", cmd->redirect_err);
	TEST_ASSERT_EQUAL_INT(cmd->append_err, 1);
}

void test_quote_mismatch(void) {
	char input[] = "echo \"hello world";
	lexer(input, cmd, arena);
	TEST_ASSERT_EQUAL_STRING("echo", cmd->argv[0]);
	TEST_ASSERT_NULL(cmd->argv[1]);
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
	RUN_TEST(test_double_quotes_dont_split_space);
	RUN_TEST(test_single_quotes_dont_split_space);
	RUN_TEST(test_single_quotes_suppress_var_expansion);
	RUN_TEST(test_double_quotes_allows_var_expansion);
	RUN_TEST(test_unquoted_quoted_mixes);
	RUN_TEST(test_redirect_out);
	RUN_TEST(test_redirect_out_append);
	RUN_TEST(test_redirect_in);
	RUN_TEST(test_redirect_err);
	RUN_TEST(test_redirect_err_append);
	RUN_TEST(test_quote_mismatch);
	return UNITY_END();
}
