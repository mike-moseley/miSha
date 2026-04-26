#ifndef ENV_H
#define ENV_H

int initEnv(void);
char *getEnv(char *key);
int setEnv(char *key, char *value);
void removeEnv(char *key);

#endif

