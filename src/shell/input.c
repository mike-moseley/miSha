#include "shell/input.h"
#include "shell/history.h"
#include "vendor/data-structures/slice.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

static struct termios orig_term;

int readline_raw(slice_t *input) {
	unsigned char c;
	size_t input_idx;
	size_t history_idx;
	history_idx = getHistoryLen();

	writePrompt();

	input_idx = 0;

	do {
		if(read(STDIN_FILENO, &c, 1) == 0) return -1;
		switch(c) {
		case 3: {
			input_idx = 0;
			clearLine();
			write(STDOUT_FILENO, "Type `exit` to exit.\n", 21);
			writePrompt();
			break;
		}
		case '\n':
			/* Continue to break out */
			continue;
		case '\b':
		case 127: {
			if(input_idx > 0) {
				input_idx--;
				input->len--;
				write(STDOUT_FILENO, "\b \b", 3);
			}
			break;
		}
		case 033: {
			char arrow_buf[2];
			int step;
			read(STDIN_FILENO, arrow_buf, 2);
			if(arrow_buf[0] == '[') {
				step = 0;
				if(arrow_buf[1] == 'A') {
					step = -1;
				} else if(arrow_buf[1] == 'B') {
					step = 1;
				} else if(arrow_buf[1] == 'C') {
					if(input_idx < input->len ) {
						input_idx++;
						input->len++;
						moveCursorRight();
					}
				} else if(arrow_buf[1] == 'D') {
					if(input_idx > 0) {
						input_idx--;
						input->len--;
						moveCursorLeft();
					}
				}

				if(step != 0 && getHistoryLen() != 0) {
					clearLine();
					writePrompt();

					history_idx = traverseHistoryIdx(history_idx, step);
					if(history_idx == SIZE_MAX) return -1;

					strcpy(input->arr, getHistoryEntry(history_idx));
					input->len = strlen(input->arr);
					input_idx = input->len;
					write(STDOUT_FILENO, input->arr, input->len);
				}
			}
			break;
		}
		default: {
			write(STDOUT_FILENO, &c, 1);
			((char *)input->arr)[input_idx] = c;
			input_idx++;
			input->len++;
		}
		}
	} while(c != '\n');
	write(STDOUT_FILENO, "\n", 1);
	((char *)input->arr)[input_idx] = '\0';
	return 0;
}

void enableRawMode(void) {
	struct termios term;
	tcgetattr(STDIN_FILENO, &orig_term);
	term = orig_term;
	term.c_lflag &= ~(ICANON | ECHO | ISIG);
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &term);
}

void disableRawMode(struct termios *term) {
	if(term == NULL) return;
	tcsetattr(STDIN_FILENO, TCSAFLUSH, term);
}

void restoreTerminal(void) { disableRawMode(&orig_term); }

void clearLine(void) { write(STDOUT_FILENO, "\r\033[K", 4); }
void moveCursorLeft(void) { write(STDOUT_FILENO, "\033[1D", 5); }
void moveCursorRight(void) { write(STDOUT_FILENO, "\033[1C", 5); }
void writePrompt(void) { write(STDOUT_FILENO, "$ ", 2); }
