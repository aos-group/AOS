#include "fifo.h"

void fifo32_init(struct FIFO32* fifo, int size, int* buf, struct TASK* task) {
	fifo->size = size;
	fifo->buf = buf;
	fifo->free = size;
	fifo->flags = 0;
	fifo->head = 0;
	fifo->tail = 0;
	fifo->task = task;
}

int fifo32_put(struct FIFO32* fifo, int data) {
	if (fifo->free == 0) {
		fifo->flags |= FLAGS_OVERRUN;
		return -1;
	}
	fifo->buf[fifo->head] = data;
	fifo->head++;
	if (fifo->head == fifo->size) 
		fifo->head = 0;
	fifo->free--;
	if (fifo->task) {
		if (fifo->task->flags != 2) {
			task_run(fifo->task);
		}
	}
	return 0;
}

int fifo32_get(struct FIFO32* fifo) {
	int data;
	if (fifo->free == fifo->size)
		return -1;
	data = fifo->buf[fifo->tail];
	fifo->tail++;
	if (fifo->tail == fifo->size)
		fifo->tail = 0;
	fifo->free++;
	return data;
}

int fifo32_status(struct FIFO32* fifo) {
	return fifo->size - fifo->free;
}
