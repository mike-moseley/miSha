#include "shell/parser.h"
#include "unity.h"
#include "vendor/alloc/arena.h"
#include "vendor/alloc/pool.h"
#include "shell/consts.h"

pool_t *pool;
arena_t *arena;

void setUp(void) {
	poolCreate(MAX_ARGS, sizeof(command_t), &pool);
	arenaCreate(MAX_ARGS * sizeof(char *) * PIPE_DEPTH, &arena);
}

void tearDown(void) {
	poolDestroy(pool);
	arenaDestroy(arena);
}

void test_single_command_success(void) {
	char input[]="ls -la";
	command_t *cmd;
	cmd = parseCommands(input, pool, arena);
	TEST_ASSERT_EQUAL_STRING("ls", cmd->argv[0]);
	TEST_ASSERT_EQUAL_STRING("-la", cmd->argv[1]);
	TEST_ASSERT_NULL(cmd->argv[2]);
	TEST_ASSERT_NULL(cmd->next);
}

void test_double_command(void) {
	char input[]="ls | grep main";
	command_t *cmd;
	cmd = parseCommands(input, pool, arena);
	TEST_ASSERT_EQUAL_STRING("ls", cmd->argv[0]);
	TEST_ASSERT_NOT_NULL(cmd->next);
	TEST_ASSERT_EQUAL_STRING("grep", cmd->next->argv[0]);
	TEST_ASSERT_EQUAL_STRING("main", cmd->next->argv[1]);
	TEST_ASSERT_NULL(cmd->argv[1]);
	TEST_ASSERT_NULL(cmd->next->argv[2]);
	TEST_ASSERT_NULL(cmd->next->next);
}

void test_triple_command(void) {
	char input[]="ls | grep main | wc -l";
	command_t *cmd;
	cmd = parseCommands(input, pool, arena);
	TEST_ASSERT_EQUAL_STRING("ls", cmd->argv[0]);
	TEST_ASSERT_NOT_NULL(cmd->next);
	TEST_ASSERT_EQUAL_STRING("grep", cmd->next->argv[0]);
	TEST_ASSERT_EQUAL_STRING("main", cmd->next->argv[1]);
	TEST_ASSERT_NOT_NULL(cmd->next->next);
	TEST_ASSERT_EQUAL_STRING("wc", cmd->next->next->argv[0]);
	TEST_ASSERT_EQUAL_STRING("-l", cmd->next->next->argv[1]);
	TEST_ASSERT_NULL(cmd->argv[1]);
	TEST_ASSERT_NULL(cmd->next->argv[2]);
	TEST_ASSERT_NULL(cmd->next->next->argv[2]);
	TEST_ASSERT_NULL(cmd->next->next->next);
}

int main(void)
{
	UNITY_BEGIN();
	RUN_TEST(test_single_command_success);
	RUN_TEST(test_double_command);
	RUN_TEST(test_triple_command);
	return UNITY_END();
}
