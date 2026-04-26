#include "mmapAlloc.h"
#include "alloc_common.h"
#include "alloc_error.h"
#include <string.h>
#include <sys/mman.h>

typedef struct block_t {
	size_t size;
} block_t;

AllocError mmapAlloc(size_t size, void **out) {
	block_t *block;
	size_t block_size;

	if(size == 0) return ALLOC_ERROR_SIZE;
	if(out == NULL) return ALLOC_ERROR_NULL;

	block_size = ALIGN(size) + sizeof(block_t);
	block = (block_t *)mmap(NULL, block_size, PROT_READ | PROT_WRITE,
	                        MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

	if(block == MAP_FAILED) return ALLOC_ERROR_OOM;

	block->size = block_size;

	/* Point to data, not to header */
	*out = (char *)block + sizeof(block_t);

	return ALLOC_OK;
}

AllocError mmapFree(void *ptr) {
	block_t *block;
	if(ptr == NULL) return ALLOC_ERROR_NULL;

	block = (block_t *)((char *)ptr - sizeof(block_t));

	munmap(block, block->size);
	return ALLOC_OK;
}

AllocError mmapRealloc(void *ptr, size_t size, void **out) {
	block_t *block;
	block_t *new_block;
	size_t new_block_size;
	AllocError err;

	if(ptr == NULL) return ALLOC_ERROR_NULL;
	if(out == NULL) return ALLOC_ERROR_NULL;

	new_block_size = ALIGN(size);
	block = (block_t *)((char *)ptr - sizeof(block_t));
	if(block->size - sizeof(block_t) >= new_block_size) {
		*out = ptr;
		return ALLOC_OK;
	}

	err = mmapAlloc(new_block_size, (void *)&new_block);
	if(err != ALLOC_OK) return err;

	memcpy(new_block, (char *)block + sizeof(block_t), block->size - sizeof(block_t));

	err = mmapFree(ptr);
	if(err != ALLOC_OK) return err;

	*out = new_block;

	return ALLOC_OK;
}
