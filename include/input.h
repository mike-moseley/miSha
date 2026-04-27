#ifndef INPUT_H
#define INPUT_H

#include <stddef.h>
#include <termios.h>

int readline_raw(char *buf, size_t size);
void enableRawMode(void);
void disableRawMode(struct termios *termios_p);
void restoreTerminal(void);
void writePrompt(void);
void clearLine(void);

#endif
