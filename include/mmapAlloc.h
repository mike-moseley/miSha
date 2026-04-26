#ifndef MMAPALLOC_H
#define MMAPALLOC_H

#include <stddef.h>
#include "alloc_error.h"

AllocError mmapAlloc(size_t size, void **out);
AllocError mmapFree(void *ptr);
AllocError mmapRealloc(void *ptr, size_t size, void **out);

#endif
