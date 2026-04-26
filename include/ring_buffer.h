#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include "cds_types.h"

typedef struct RingBuffer {
	void *buf;
	size_t len;
	size_t cap;
	size_t head;
	size_t tail;
	size_t element_size;
} ringbuffer_t;

ringbuffer_t *createRingBuffer(size_t initial_cap, size_t element_size);
cds_err_t freeRingBuffer(ringbuffer_t *ringbuffer);
cds_err_t pushRingBuffer(ringbuffer_t *ringbuffer,void *element);
void *popRingBuffer(ringbuffer_t *ringbuffer);
void *peekRingBuffer(ringbuffer_t *ringbuffer);
void *getFromRingBuffer(ringbuffer_t *ringbuffer, size_t idx);

#endif
