#ifndef HASH_MAP_H
#define HASH_MAP_H

#include "cds_types.h"

typedef struct HashNode {
	void *key;
	void *value;
	struct HashNode *next;
} hnode_t;

typedef struct HashMap {
	hnode_t **buckets;
	size_t len;
	size_t cap;
	size_t key_size;
	size_t value_size;
} hashmap_t;

hashmap_t *createHashMap(size_t key_size, size_t value_size,size_t initial_cap);
cds_err_t freeHashMap(hashmap_t *hashmap, void(*free_data)(void *));
cds_err_t insertToHashMap(hashmap_t *hashmap, void *key, void*value);
void *getFromHashMap(hashmap_t *hashmap, void *key);
cds_err_t removeFromHashMap(hashmap_t *hashmap, void *key);

#endif
