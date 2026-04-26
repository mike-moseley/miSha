#include "env.h"
#include "alloc_error.h"
#include "hash_map.h"
#include "hash_map.h"
#include "mmapAlloc.h"
#include <stdio.h>
#include <string.h>

#define INITIAL_MAP_SIZE 56

hashmap_t *envMap;
extern char **environ;

int initEnv(void) {
	int i;
	envMap = createHashMap(sizeof(char *), sizeof(char *), INITIAL_MAP_SIZE);

	for(i = 0; environ[i] != NULL; i++) {
		char *delimit_ptr, *key, *value;
		void *environ_ptr, *key_ptr, *value_ptr;
		AllocError err;

		err = mmapAlloc(strlen(environ[i])+1, &environ_ptr);
		if(err != ALLOC_OK) {
			fprintf(stderr, "mmapAlloc in initEnv in env.c: AllocError=%d", err);
		};
		strcpy(environ_ptr, environ[i]);

		/* TODO: extract strchr logic to helper fn to use here and in
		 * builtins.c/builtin_export */
		delimit_ptr = strchr((char *)environ_ptr, '=');
		if(delimit_ptr == NULL) {
			fprintf(stderr, "strchr in initEnv in env.c: Malformed input");
			return -1;
		}
		*delimit_ptr = '\0';
		key = environ_ptr;
		value = delimit_ptr + 1;
		mmapFree(environ_ptr);

		err = mmapAlloc(strlen(key) + 1, &key_ptr);
		if(err != ALLOC_OK) {
			fprintf(stderr, "mmapAlloc in initEnv in env.c: AllocError=%d", err);
		};
		strcpy(key_ptr, key);

		err = mmapAlloc(strlen(value) + 1, &value_ptr);
		if(err != ALLOC_OK) {
			fprintf(stderr, "mmapAlloc in initEnv in env.c: AllocError=%d", err);
		};
		strcpy(value_ptr, value);

		insertToHashMap(envMap, key_ptr, value_ptr);
	}
	
	return 0;
}
