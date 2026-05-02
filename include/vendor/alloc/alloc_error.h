#ifndef ERR_TYPE_H
#define ERR_TYPE_H

typedef enum {
	ALLOC_OK = 0,
	ALLOC_ERROR_NULL = -1,
	ALLOC_ERROR_SIZE = -2,
	ALLOC_ERROR_OOM = -3
} AllocError;

#endif
