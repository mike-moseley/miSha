#ifndef INPUT_H
#define INPUT_H

#include "vendor/data-structures/slice.h"
#include <stddef.h>
#include <termios.h>

int readline_raw(slice_t *input);
void enableRawMode(void);
void disableRawMode(struct termios *termios_p);
void restoreTerminal(void);
void writePrompt(void);
void clearLine(void);
void clearScreen(void);
void moveCursorRight(void);
void moveCursorLeft(void);
void moveCursorRightN(int);
void moveCursorLeftN(int);

#endif
