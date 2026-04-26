#ifndef ENV_H
#define ENV_H

int initEnv(void);
char *getEnv(char *key);
int setEnv(char *env);
int removeEnv(char *env);

#endif

