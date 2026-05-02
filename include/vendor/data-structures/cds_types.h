#ifndef CDS_TYPES_H
#define CDS_TYPES_H

#include <stddef.h>

typedef enum {
	CDS_OK = 0,
	CDS_ERR_NULL = -1,
	CDS_ERR_OOM = -2,
	CDS_ERR_BOUNDS = -3,
	CDS_ERR_INVALID = -4,
	CDS_ERR_NOT_FOUND = -5
} cds_err_t;

#endif
