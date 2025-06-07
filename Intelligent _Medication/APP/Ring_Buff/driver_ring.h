#ifndef __driver_ring_H__
#define __driver_ring_H__

#include "bsp_system.h"
#include "stdbool.h"

#define BUFFER_SIZE 256

typedef struct {
	uint8_t buffer[100];
	uint16_t head;
	uint16_t tail;
	uint16_t count;
}CircularBuffer;

void CircularBuffer_Init(CircularBuffer *cb);
void CircularBuffer_Put(CircularBuffer *cb, char *data);
void CircularBuffer_Get(CircularBuffer *cb, char *data);
bool isBufferEmpty(CircularBuffer* buff);
bool isBufferFill(CircularBuffer* buff);

#endif
