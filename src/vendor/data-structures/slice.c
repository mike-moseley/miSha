#include "vendor/data-structures/slice.h"
#include "vendor/data-structures/cds_types.h"
#include <stdlib.h>
#include <string.h>

slice_t *createSlice(size_t element_size, size_t initial_len) {
	slice_t *slice;
	size_t initial_cap;
	void *arr;

	if(initial_len == 0) {
		initial_cap = 1;
	} else {
		initial_cap = initial_len + initial_len / 2;
	}
	arr = calloc(initial_cap, element_size);
	if(arr == NULL) {
		return NULL;
	}

	slice = malloc(sizeof(slice_t));
	if(slice == NULL) {
		return NULL;
	}
	slice->arr = arr;
	slice->element_size = element_size;
	slice->len = initial_len;
	slice->cap = initial_cap;

	return slice;
}

cds_err_t growSlice(slice_t *slice) {
	size_t new_cap;
	void *new_arr;

	new_cap = slice->cap + slice->cap / 2;
	if (new_cap == slice->cap) new_cap = slice->cap + 1; 
	new_arr = realloc(slice->arr, new_cap * slice->element_size);
	if(new_arr == NULL) {
		return CDS_ERR_OOM;
	}
	slice->cap = new_cap;
	slice->arr = new_arr;

	return CDS_OK;
}

cds_err_t appendSlice(slice_t *slice, void *data) {
	int err;
	if(slice == NULL) {
		return CDS_ERR_NULL;
	}
	if(data == NULL) {
		return CDS_ERR_NULL;
	}
	if(slice->len >= slice->cap) {
		err = growSlice(slice);
		if(err != CDS_OK) {
			return err;
		}
	}
	memcpy((char *)slice->arr + (slice->len * slice->element_size), data,
	       slice->element_size);
	slice->len++;
	return CDS_OK;
}

cds_err_t appendSliceArr(slice_t *slice, void **data_arr, size_t len) {
	size_t i;
	cds_err_t err;

	if(slice==NULL) return CDS_ERR_NULL;
	if(data_arr == NULL) return CDS_ERR_NULL;
	for(i=0; i < len; i++) {
		err = appendSlice(slice, data_arr[i]);
		/* TODO: Improve this to roll back appends if we error */
		if (err != CDS_OK) {
			return err;
		}
	}
	return CDS_OK;
}

slice_t *subSlice(slice_t *slice, size_t start_idx, size_t end_idx) {
	slice_t *new_slice;
	size_t num_entries;
	void *new_arr;

	if(slice == NULL) {
		return NULL;
	}
	if(start_idx >= end_idx) {
		return NULL;
	}
	if(end_idx > slice->len) {
		return NULL;
	}

	num_entries = end_idx - start_idx;
	new_arr = calloc(num_entries, slice->element_size);
	if(new_arr == NULL) {
		return NULL;
	}
	memcpy(new_arr, (char *)slice->arr + (start_idx * slice->element_size),
	       num_entries * slice->element_size);
	new_slice = malloc(sizeof(slice_t));
	if(new_slice == NULL) {
		free(new_arr);
		return NULL;
	}

	new_slice->element_size = slice->element_size;
	new_slice->arr = new_arr;
	new_slice->len = num_entries;
	new_slice->cap = num_entries + num_entries / 2;

	return new_slice;
}

cds_err_t freeSlice(slice_t *slice, void (*free_data)(void *)) {
	int i;
	char *arr;
	if (slice == NULL) {
		return CDS_ERR_NULL;
	}
	if (free_data == NULL) {
		free(slice->arr);
		free(slice);
		return CDS_OK;
	} 
	
	arr = (char *)slice->arr;
	for (i=0; i<(int)slice->len; i++) {
		free_data(arr + i * slice->element_size);
	}
	free(slice->arr);
	free(slice);
	return CDS_OK; 
}
cds_err_t insertToSlice(slice_t *slice, void *data, size_t idx) {
	char *arr_idx;
	char *shifted_idx;
	int err;
	if (slice == NULL) {
		return CDS_ERR_NULL;
	}
	if (data == NULL) {
		return CDS_ERR_NULL;
	}
	if (idx > slice->len) {
		return CDS_ERR_INVALID;
	}
	if (slice->len >= slice->cap) {
		err = growSlice(slice);

		if (err != CDS_OK) return err;
	}
	arr_idx = ((char *)slice->arr + idx * slice->element_size);
	shifted_idx = (char *)slice->arr + (idx+1) * slice->element_size;
	memmove(shifted_idx,arr_idx,(slice->len - idx)*slice->element_size);
	memcpy(arr_idx, data, slice->element_size);
	slice->len++;
	return CDS_OK;
}
cds_err_t removeElement(slice_t *slice, size_t idx, void(*free_data)(void *)) { 
	void *element_ptr;
	void *move_ptr;
	if (slice == NULL) {
		return CDS_ERR_NULL;
	}
	if (idx >= slice->len) {
		return CDS_ERR_INVALID;
	}
	element_ptr = (char *)slice->arr + idx * slice->element_size;
	move_ptr = (char *)slice->arr + (idx+1)* slice->element_size;
	if (free_data != NULL) {
		free_data(element_ptr);
	} 
	memmove(element_ptr, move_ptr,(slice->len - idx - 1) * slice->element_size);
	slice->len--;

	return CDS_OK;
}
void *getFromSlice(slice_t *slice, size_t idx) { 
	if (slice == NULL) return NULL; 

	return ((char *)slice->arr + idx * slice->element_size);
}
