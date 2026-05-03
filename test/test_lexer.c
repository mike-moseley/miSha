#include "shell/consts.h"
#include "shell/env.h"
#include "shell/lexer.h"
#include "unity.h"
#include "unity_internals.h"
#include <stdlib.h>

void setUp(void) {}
void tearDown(void) {}

void test_ok(void) {
	char *argv[MAX_ARGS];
	char input[] = "ls -la";
	lexer(input, argv);
	TEST_ASSERT_EQUAL_STRING("ls", argv[0]);
	TEST_ASSERT_EQUAL_STRING("-la", argv[1]);
	TEST_ASSERT_NULL(argv[2]);
}

void test_num_tokens(void) {
	char *argv[MAX_ARGS];
	char input[] = "echo hello world";
	lexer(input, argv);
	TEST_ASSERT_NOT_NULL(argv[0]);
	TEST_ASSERT_NOT_NULL(argv[1]);
	TEST_ASSERT_NOT_NULL(argv[2]);
	TEST_ASSERT_NULL(argv[3]);
}

void test_empty(void) {
	char *argv[MAX_ARGS];
	char input[] = "";
	lexer(input,argv);
	TEST_ASSERT_NULL(argv[0]);
}

void test_whitespace(void) {
	char *argv[MAX_ARGS];
	char input[] = " ls\t-la     ";
	lexer(input,argv);
	TEST_ASSERT_EQUAL_STRING("ls", argv[0]);
	TEST_ASSERT_EQUAL_STRING("-la", argv[1]);
	TEST_ASSERT_NULL(argv[2]);
}

void test_env_var_expansion(void) {
	char *argv[MAX_ARGS];
	char input[] = "echo $HOME";
	lexer(input,argv);
	TEST_ASSERT_EQUAL_STRING("echo", argv[0]);
	TEST_ASSERT_EQUAL_STRING(getenv("HOME"), argv[1]);
	TEST_ASSERT_NULL(argv[2]);
}

void test_undefined_env_var(void) {
	char *argv[MAX_ARGS];
	char input[] = "echo $BRAND_NEW_ENV_VAR";
	lexer(input,argv);
	TEST_ASSERT_EQUAL_STRING("echo", argv[0]);
	TEST_ASSERT_EQUAL_STRING("", argv[1]);
	TEST_ASSERT_NULL(argv[2]);
}

int main(void) {
	initEnv();
	UNITY_BEGIN();
	RUN_TEST(test_ok);
	RUN_TEST(test_num_tokens);
	RUN_TEST(test_empty);
	RUN_TEST(test_whitespace);
	RUN_TEST(test_env_var_expansion);
	RUN_TEST(test_undefined_env_var);
	return UNITY_END();
}
