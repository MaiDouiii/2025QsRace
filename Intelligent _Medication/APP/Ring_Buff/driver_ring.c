#include "driver_ring.h"

void CircularBuffer_Init(CircularBuffer *cb) {
    cb->head = 0;
    cb->tail = 0;
    cb->count = 0;
}

void CircularBuffer_Put(CircularBuffer *cb, char *data) {
	uint16_t data_len = strlen((char *)data);
	for(uint16_t i = 0; i<data_len ; i++){
		if (cb->count < BUFFER_SIZE) {
			cb->buffer[cb->head] = data[i];
			cb->head = (cb->head + 1) % BUFFER_SIZE;
			cb->count++;
		}else{
			break;
		}
    }
}

void CircularBuffer_Get(CircularBuffer *cb, char *data) {
	uint16_t sy = 0;
    while (cb->count > 0) {
        data[sy] = cb->buffer[cb->tail];
        cb->tail = (cb->tail + 1) % BUFFER_SIZE;
        cb->count--;
		sy++;
    }
}

bool isBufferEmpty(CircularBuffer* buff){
	return buff->count == 0;
}

bool isBufferFill(CircularBuffer* buff){
	return buff->count == BUFFER_SIZE;
}
