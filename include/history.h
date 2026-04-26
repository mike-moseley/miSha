#ifndef HISTORY_H
#define HISTORY_H
#include "cds_types.h"

cds_err_t initHistory(void);
cds_err_t pushHistory(char *command);
void printHistory(void);

#endif
