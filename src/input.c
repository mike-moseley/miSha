#include "input.h"
#include "history.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

static struct termios orig_term;

void clearLine(void) {
	write(STDOUT_FILENO, "\r\033[K", 4);
}
void writePrompt(void) {
	write(STDOUT_FILENO, "$ ", 2);
}

int readline_raw(char *buf, size_t size) {
	unsigned char c;
	size_t buf_idx;
	size_t history_idx;
	history_idx = getHistoryLen();

	writePrompt();

	if(buf == NULL) {
			fprintf(stderr, "ERROR: readline_raw in input.c\n");
			fprintf(stderr, "  buf is NULL\n");
			return -1;
	}

	if(size == 0) {
			fprintf(stderr, "ERROR: readline_raw in input.c\n");
			fprintf(stderr, "  size is 0\n");
			return -1;
	}

	buf_idx = 0;

	do {
		if (buf_idx > size) {
			fprintf(stderr, "ERROR: readline_raw in input.c\n");
			fprintf(stderr, "  buf_idx > size\n");
			return -1;
		}
		if(read(STDIN_FILENO, &c, 1) == 0) return -1;
		switch(c) {
			case 3: {
				buf_idx = 0;
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
				if(buf_idx > 0) {
					buf_idx--;
					write(STDOUT_FILENO, "\b \b", 3);
				}
				break;
			}
			case 033: {
				char arrow_buf[2];
				int step;
				read(STDIN_FILENO,arrow_buf,2);
				if(arrow_buf[0] == '[') {
					step = 0;
					if(arrow_buf[1] == 'A') {
						step = -1;
					} else if (arrow_buf[1] == 'B') {
						step = 1;
					}

					if (step != 0 && getHistoryLen() != 0) {
						clearLine();
						writePrompt();

						history_idx = traverseHistoryIdx(history_idx, step);
						if (history_idx == SIZE_MAX) return -1;

						strcpy(buf, getHistoryEntry(history_idx));
						buf_idx = strlen(buf);
						write(STDOUT_FILENO, buf, strlen(buf));
					}
				}
				break;
			}
			default: {
				write(STDOUT_FILENO, &c, 1);
				buf[buf_idx] = c;
				buf_idx++;
			}
		}
	} while(c != '\n');
	write(STDOUT_FILENO, "\n", 1);
	buf[buf_idx] = '\0';
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
	if (term == NULL) return;
	tcsetattr(STDIN_FILENO, TCSAFLUSH, term);
}

void restoreTerminal(void) {
	disableRawMode(&orig_term);
}

