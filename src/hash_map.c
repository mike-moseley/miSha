#include "hash_map.h"
#include "cds_types.h"
#include <bits/stdint-intn.h>
#include <stdlib.h>
#include <string.h>

static int keycmp(hashmap_t *hashmap, void *bucket_key, void *key) {
	int matched;
	if(hashmap->key_size == 0) {
		matched = strcmp(key, bucket_key) == 0;
	} else {
		matched = memcmp(key, bucket_key, hashmap->key_size) == 0;
	}
	return matched;
}

/* FNV-1a hash */
static size_t hash_function(hashmap_t *hashmap, void *key) {
	size_t hash;
	size_t i;
	unsigned char *bytes;
	/* key is not a string */
	if(hashmap->key_size > 0) {
		hash = 2166136261u;
		bytes = (unsigned char *)key;
		for(i = 0; i < hashmap->key_size; i++) {
			hash ^= *(bytes + i);
			hash *= 16777619u;
		}
	} else {
		bytes = (unsigned char *)key;
		hash = 2166136261u;
		while(*(bytes) != '\0') {
			hash ^= *(bytes);
			hash *= 16777619u;
			bytes++;
		}
	}
	return hash % hashmap->cap;
}

static cds_err_t resizeHashMap(hashmap_t *hashmap) {
	hnode_t **new_buckets;
	hnode_t *bucket;
	hnode_t *next_bucket;
	size_t old_cap;
	size_t new_hash;
	size_t i;

	old_cap = hashmap->cap;
	new_buckets = calloc(hashmap->cap * 2, sizeof(hnode_t *));
	if(new_buckets == NULL) {
		return CDS_ERR_OOM;
	}
	hashmap->cap *= 2;

	for(i = 0; i < old_cap; i++) {
		bucket = hashmap->buckets[i];
		if(bucket == NULL) {
			continue;
		}
		while(bucket != NULL) {
			new_hash = hash_function(hashmap, bucket->key);
			next_bucket = bucket->next;
			bucket->next = new_buckets[new_hash];
			new_buckets[new_hash] = bucket;
			bucket = next_bucket;
		}
	}
	free(hashmap->buckets);
	hashmap->buckets = new_buckets;
	return CDS_OK;
}

hashmap_t *createHashMap(size_t key_size, size_t value_size,
                         size_t initial_cap) {
	hashmap_t *hashmap;
	hnode_t **buckets;

	if(initial_cap == 0) {
		return NULL;
	}
	hashmap = malloc(sizeof(hashmap_t));
	if(hashmap == NULL) {
		return NULL;
	}
	buckets = calloc(initial_cap, sizeof(hnode_t *));
	if(buckets == NULL) {
		free(hashmap);
		return NULL;
	}
	hashmap->buckets = buckets;
	hashmap->len = 0;
	hashmap->cap = initial_cap;
	hashmap->key_size = key_size;
	hashmap->value_size = value_size;

	return hashmap;
}

cds_err_t freeHashMap(hashmap_t *hashmap, void (*free_data)(void *)) {
	size_t i;
	hnode_t *bucket;
	hnode_t *next_bucket;
	hnode_t *old_bucket;
	if(hashmap == NULL) {
		return CDS_ERR_NULL;
	}
	for(i = 0; i < hashmap->cap; i++) {
		if(hashmap->buckets[i] == NULL) {
			continue;
		}

		bucket = hashmap->buckets[i];
		next_bucket = bucket->next;
		while(bucket != NULL) {
			free(bucket->key);
			free_data(bucket->value);
			old_bucket = bucket;
			bucket = next_bucket;
			if(bucket != NULL) {
				next_bucket = bucket->next;
			}
			free(old_bucket);
		}
	}
	free(hashmap->buckets);
	free(hashmap);
	return CDS_OK;
}

cds_err_t insertToHashMap(hashmap_t *hashmap, void *key, void *value) {
	size_t hash;
	int found;
	hnode_t *hash_node;
	hnode_t *current_bucket;
	void *node_key;
	void *node_value;

	if(hashmap == NULL) {
		return CDS_ERR_NULL;
	}
	if(key == NULL) {
		return CDS_ERR_NULL;
	}
	if(value == NULL) {
		return CDS_ERR_NULL;
	}

	found = 0;
	hash = hash_function(hashmap, key);
	hash_node = malloc(sizeof(hnode_t));
	if(hash_node == NULL) {
		return CDS_ERR_OOM;
	}
	node_key = malloc(hashmap->key_size);
	if(node_key == NULL) {
		free(hash_node);
		return CDS_ERR_OOM;
	}
	node_value = malloc(hashmap->value_size);
	if(node_value == NULL) {
		free(node_key);
		free(hash_node);
		return CDS_ERR_OOM;
	}
	memcpy(node_key, key, hashmap->key_size);
	memcpy(node_value, value, hashmap->value_size);
	hash_node->key = node_key;
	hash_node->value = node_value;
	hash_node->next = NULL;
	if(hashmap->buckets[hash] == NULL) {
		hashmap->buckets[hash] = hash_node;
		hashmap->len++;
	} else {
		current_bucket = hashmap->buckets[hash];
		while(current_bucket->next != NULL) {
			if(keycmp(hashmap, current_bucket->key, key)) {
				memcpy(current_bucket->value, value, hashmap->value_size);
				free(hash_node);
				free(node_key);
				free(node_value);
				found = 1;
				break;
			} else {
				current_bucket = current_bucket->next;
			}
		}
		if(!found) {
			if(keycmp(hashmap, current_bucket->key, key)) {
				memcpy(current_bucket->value, value, hashmap->value_size);
				found = 1;
				free(hash_node);
				free(node_key);
				free(node_value);
			} else {
				current_bucket->next = hash_node;
				hashmap->len++;
			}
		}
	}
	if (hashmap->len * 10 > hashmap->len * 7) {
		resizeHashMap(hashmap);
	}
	return CDS_OK;
}
void *getFromHashMap(hashmap_t *hashmap, void *key) {
	size_t hash;
	hnode_t *bucket;

	if(hashmap == NULL) {
		return NULL;
	}
	if(key == NULL) {
		return NULL;
	}

	hash = hash_function(hashmap, key);
	bucket = hashmap->buckets[hash];

	while(bucket != NULL) {
		if(keycmp(hashmap, bucket->key, key)) {
			return bucket->value;
		} else {
			bucket = bucket->next;
		}
	}
	return NULL;
}
cds_err_t removeFromHashMap(hashmap_t *hashmap, void *key) {
	size_t hash;
	hnode_t *bucket;
	hnode_t *prev_bucket;

	if(hashmap == NULL) {
		return CDS_ERR_NULL;
	}
	if(key == NULL) {
		return CDS_ERR_NULL;
	}

	hash = hash_function(hashmap, key);
	bucket = hashmap->buckets[hash];
	prev_bucket = NULL;

	while(bucket != NULL) {
		if(keycmp(hashmap, bucket->key, key)) {
			free(bucket->key);
			free(bucket->value);
			if(prev_bucket != NULL) {
				prev_bucket->next = bucket->next;
			} else {
				hashmap->buckets[hash] = bucket->next;
			}
			free(bucket);
			hashmap->len--;
			return CDS_OK;
		} else {
			prev_bucket = bucket;
			bucket = bucket->next;
		}
	}
	return CDS_ERR_NOT_FOUND;
}
