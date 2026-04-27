#ifndef HISTORY_H
#define HISTORY_H
#include "cds_types.h"

cds_err_t initHistory(void);
cds_err_t pushHistory(char *command);
void printHistory(void);
char *getHistoryEntry(size_t idx);
size_t getHistoryLen(void);
size_t traverseHistoryIdx(size_t idx, int step);

#endif
