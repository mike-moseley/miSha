#include "shell/consts.h"
#include "shell/executor.h"
#include "shell/parser.h"
#include "unity.h"
#include "unity_internals.h"
#include <fcntl.h>
#include <sys/wait.h>
#include <termios.h>
#include <unistd.h>

/*
 * We need to save & restore the terminal manually since that isnt handled
 * in executor.c
 */
struct termios saved;
pool_t *pool;
arena_t *arena;
int stderr_fd;

void setUp(void) {
	int devnull;
	tcgetattr(STDIN_FILENO, &saved);
	poolCreate(MAX_ARGS, sizeof(command_t), &pool);
	arenaCreate((MAX_ARGS * sizeof(char *)
			 + (MAX_ARGS * BUF_SIZE))
			 * PIPE_DEPTH, &arena);
	/* Redirect stderr so it doesn't mess up output of tests*/
	stderr_fd = dup(STDERR_FILENO);
	devnull = open("/dev/null", O_WRONLY);
	dup2(devnull, STDERR_FILENO);
	close(devnull);
}

void tearDown(void) {
	poolDestroy(pool);
	arenaDestroy(arena);
	dup2(stderr_fd, STDERR_FILENO);
	close(stderr_fd);
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &saved);
}

void test_null(void) {
	int result;
	command_t *cmd = NULL;
	result = execute(cmd);
	TEST_ASSERT_EQUAL(-1, result);
}

void test_single_command_success(void) {
	int result;
	char *argv[] = {"true", NULL};
	command_t cmd;
	cmd.next = NULL;
	cmd.argv = argv;
	result = execute(&cmd);
	TEST_ASSERT_EQUAL(WEXITSTATUS(result), 0);
}

void test_single_command_fail(void) {
	int result;
	char *argv[] = {"false", NULL};
	command_t cmd;
	cmd.next = NULL;
	cmd.argv = argv;
	result = execute(&cmd);
	TEST_ASSERT_NOT_EQUAL(WEXITSTATUS(result), 0);
}

void test_single_command_dne(void) {
	int result;
	char *argv[] = {"/not_a_binary", NULL};
	command_t cmd;
	cmd.next = NULL;
	cmd.argv = argv;
	result = execute(&cmd);
	TEST_ASSERT_NOT_EQUAL(WEXITSTATUS(result), 0);
}

void test_two_commands(void) {
	int result;
	int stdout_fd;
	int stdout_pipe[2];
	int bytes;
	char buf[56];
	command_t *cmd;
	char input[] = "echo hello | cat";
	/* this doesn't work in tearDown(), I don't really understand
	 * why. */
	fflush(stdout);
	stdout_fd = dup(STDOUT_FILENO);
	if(pipe(stdout_pipe) != -1) {
		dup2(stdout_pipe[1], STDOUT_FILENO);
		close(stdout_pipe[1]);
	}

	cmd = parseCommands(input, pool, arena);
	result = execute(cmd);
	bytes = read(stdout_pipe[0], buf, sizeof(buf));
	/* null terminate buf */
	buf[bytes] = '\0';
	dup2(stdout_fd, STDOUT_FILENO);
	TEST_ASSERT_EQUAL(WEXITSTATUS(result), 0);
	TEST_ASSERT_EQUAL_STRING("hello\n", buf);
	close(stdout_fd);
	close(stdout_pipe[0]);
}

int main(void) {
	UNITY_BEGIN();
	RUN_TEST(test_null);
	RUN_TEST(test_single_command_success);
	RUN_TEST(test_single_command_fail);
	RUN_TEST(test_single_command_dne);
	RUN_TEST(test_two_commands);
	return UNITY_END();
}
