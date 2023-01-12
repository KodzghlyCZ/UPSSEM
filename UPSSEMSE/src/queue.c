#include "queue.h"

int createMessageQueue(int queueSize, message_queue_t * queue)
{
	queue->data = calloc(queueSize, sizeof(message_t));
	queue->size = queueSize;
	queue->current = 0;

	return 0;
}

void deleteMessageQueue(message_queue_t * queue)
{
	free(queue->data);
	queue->data = NULL;
}

int enqueue(message_queue_t * queue, message_t * message)
{
	if (queue->current == queue->size)
		return -1;

	memcpy(&queue->data[queue->current], message, sizeof(message_t));
	queue->current++;

	return 0;
}

int dequeue(message_queue_t * queue, message_t * message)
{
	if (queue->current == 0)
		return -1;

	memcpy(message, &queue->data[0], sizeof(message_t));
	memmove(&queue->data[0], &queue->data[1],
		(queue->current - 1) * sizeof(message_t));
	queue->current--;

	return 0;
}

int dequeueAll(message_queue_t * queue)
{
	queue->current = 0;

	return 0;
}
