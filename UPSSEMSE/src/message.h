#ifndef MESSAGE_H_
#define MESSAGE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

//Velikost jedne casti zpravy
#define MESSAGE_PART_SIZE 8

//Pocet casti jedne zpravy
#define MESSAGE_PARTS 3

//Velikost jedne zpravy
#define MESSAGE_SIZE 24

//Prah poctu nesmyslnych prikazu pred odpojenim.
#define NONSENSE_THRESHOLD 10

//Maximalni velikost fronty odesilanych zprav
#define MAX_MESSAGES_BUFFER_SIZE 40

//Definice struktury zpravy
typedef struct {
	char data[MESSAGE_SIZE];
}  message_t;

//Funkce pripravy zpravy retezce 'data' do zpravy 'message'
int prepareMessage(char *data, message_t *message);

//Funkce vypsani zpravy 'message'
int printMessage(message_t *message);

#endif /* MESSAGE_H */ 
