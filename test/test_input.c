#include "shell/history.h"
#include "shell/input.h"
#include "unity.h"
#include "unity_internals.h"
#include "vendor/data-structures/slice.h"
#include <string.h>
#include <unistd.h>

int stdin_fd;
int stdin_pipe[2];

int stdout_fd;
int stdout_pipe[2];

slice_t *input;

void setUp(void) {
	/* Store STDIN_FILENO and STDOUT_FILENO file descriptors
	 * so that they can be restored in tearDown. */
	stdin_fd = dup(STDIN_FILENO);
	stdout_fd = dup(STDOUT_FILENO);
	input = createSlice(sizeof(char), 0);
	if(pipe(stdin_pipe) != -1 && pipe(stdout_pipe) != -1) {
		dup2(stdin_pipe[0], STDIN_FILENO);
		dup2(stdout_pipe[1], STDOUT_FILENO);
		/* Close the file descriptors created by pipe() as they
		 * are now stored in STDIN_FILENO and STDOUT_FILENO */
		close(stdin_pipe[0]);
		close(stdout_pipe[1]);
	}
}

void tearDown(void) {
	dup2(stdin_fd, STDIN_FILENO);
	dup2(stdout_fd, STDOUT_FILENO);
	freeSlice(input, NULL);
	close(stdin_fd);
	close(stdin_pipe[1]);
	close(stdout_fd);
	close(stdout_pipe[0]);
}

void test_basic_valid(void) { 
	write(stdin_pipe[1], "hello\n", 6);
	close(stdin_pipe[1]);
	readline_raw(input);
	TEST_ASSERT_EQUAL_STRING("hello", (char *)input->arr);
}

void test_backspace(void) {
	write(stdin_pipe[1], "hello\x7f\n", 7);
	close(stdin_pipe[1]);
	readline_raw(input);
	TEST_ASSERT_EQUAL_STRING("hell", (char *)input->arr);
}

void test_backspace_empty_buffer(void) {
	write(stdin_pipe[1], "\x7f\n", 2);
	close(stdin_pipe[1]);
	readline_raw(input);
	TEST_ASSERT_EQUAL_STRING("", (char *)input->arr);
}

void test_cursor_movement_insert(void) {
	write(stdin_pipe[1], "hllo\033[D\033[D\033[De\n", 15);
	close(stdin_pipe[1]);
	readline_raw(input);
	TEST_ASSERT_EQUAL_STRING("hello", (char *)input->arr);
}

void test_cursor_movement_backspace(void) {
	write(stdin_pipe[1], "hello\033[D\033[D\033[D\x7f\n", 16);
	close(stdin_pipe[1]);
	readline_raw(input);
	TEST_ASSERT_EQUAL_STRING("hllo", (char *)input->arr);
}

void test_cursor_nonnegative(void) {
	write(stdin_pipe[1], "\033[D\n", 4);
	close(stdin_pipe[1]);
	readline_raw(input);
	TEST_ASSERT_EQUAL_STRING("", (char *)input->arr);
}

void test_history(void) { 
	write(stdin_pipe[1], "hello\n\033[A\n", 10);
	readline_raw(input);
	close(stdin_pipe[1]);
	pushHistory((char *)input->arr);
	input->len = 0;
	readline_raw(input);
	TEST_ASSERT_EQUAL_STRING("hello", (char *)input->arr);
}

void test_rawmode_ctrl_c_clears(void) {
	write(stdin_pipe[1], "hello\x03world\n", 12);
	readline_raw(input);
	close(stdin_pipe[1]);
	TEST_ASSERT_EQUAL_STRING("world", (char *)input->arr);
}

int main(void) {
	initHistory();
	UNITY_BEGIN();
	RUN_TEST(test_basic_valid);
	RUN_TEST(test_backspace);
	RUN_TEST(test_backspace_empty_buffer);
	RUN_TEST(test_cursor_movement_insert);
	RUN_TEST(test_cursor_nonnegative);
	RUN_TEST(test_cursor_movement_backspace);
	RUN_TEST(test_history);
	RUN_TEST(test_rawmode_ctrl_c_clears);
	return UNITY_END();
}
