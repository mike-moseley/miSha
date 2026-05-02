#ifndef ENV_H
#define ENV_H
#include "vendor/data-structures/cds_types.h"

cds_err_t initEnv(void);
char *getEnv(char *key);
cds_err_t setEnv(char *key, char *value);
cds_err_t removeEnv(char *key);

#endif

