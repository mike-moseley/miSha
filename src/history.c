#include "history.h"
#include "cds_types.h"
#include "consts.h"
#include "ring_buffer.h"
#include <stdint.h>
#include <stdio.h>

ringbuffer_t *history;

cds_err_t initHistory(void) {
	ringbuffer_t *tmp;
	tmp = createRingBuffer(RING_BUF_SIZE, BUF_SIZE);
	if(tmp == NULL) return CDS_ERR_OOM;
	history = tmp;
	return CDS_OK;
}

cds_err_t pushHistory(char *command) {
	cds_err_t err;
	err = pushRingBuffer(history, command);
	return err;
}

char *getHistoryEntry(size_t idx) {
	return getFromRingBuffer(history, idx);
}

size_t traverseHistoryIdx(size_t idx, int step) {
	if(history == NULL) return SIZE_MAX;
	/* Ensures negative step cannot underflow unnoticed */
	if(step < 0 && -step > (int)history->len) return SIZE_MAX;
	return ((int)idx + step + history->len) % history->len;
}

size_t getHistoryLen(void) {
	if(history == NULL) return -1;
	return history->len;
}

void printHistory(void) {
	size_t i;
	char *line;
	puts("History:");
	for(i=0; i<history->len; i++) {
		line = getFromRingBuffer(history, i);
		printf(" - %s\n", line);
	}
}
