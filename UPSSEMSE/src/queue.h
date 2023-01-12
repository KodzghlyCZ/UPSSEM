#ifndef QUEUE_H_
#define QUEUE_H_

#include "message.h"

//Struktura fronty
typedef struct {
	message_t *data;
	int size;
	int current;
} message_queue_t;

//Funkce pridani prvku 'message' do fronty 'queue'
int enqueue(message_queue_t *queue, message_t *message);

//Funkce odebrani prvku 'message' z fronty 'queue'
int dequeue(message_queue_t *queue, message_t *message);

//Funkce vytvoreni nove fronty o velikosti 'queueSize' na pointeru 'queue'
int createMessageQueue(int queueSize, message_queue_t *queue);

//Funkce smazani stavajici fronty 'queue'
void deleteMessageQueue(message_queue_t *queue);

//Funkce pro odebrani vsech prvku fronty 'queue'
int dequeueAll(message_queue_t *queue);

//Funkce pro overeni prazdne fronty 'queue'
int isEmpty(message_queue_t *queue);

//Funkce pro overeni plne fronty 'queue'
int isFull(message_queue_t *queue);

#endif
