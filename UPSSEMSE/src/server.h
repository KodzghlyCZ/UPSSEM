#ifndef SERVER_H_
#define SERVER_H_

#include "client.h"

//Maximalni pocet celkove pripojenych klientu
#define MAX_CLIENTS 10

//Hodnota stavu socketu pri prazdnem socketu
#define NO_SOCKET -1

//Funkce pro spravne ukonceni serveru s kodem 'code'
void shutdownProperly(int code);

//Funkce prijeti dat od klienta 'peer'
int receiveFromClient(client *peer);

//Funkce zaslani zprav klientovi 'peer'
int sendToClient(client *peer);

//Funkce zaslani zpravy skladajici se z 'messageType', 'messageContent' a 'messageTail' klientovi 'client'
uint8_t sendMessage(char *messageType, char *messageContent, char *messageTail, client *client);

//Funkce zaslani zpravy skladajici se z 'messageType', 'messageContent' a 'messageTail' vsem dostupnym klientum
uint8_t broadcastMessage(char *messageType, char *messageContent, char *messageTail);

//Funkce kontroly pripojeneho hrace se jmenem 'nickname'
uint8_t checkNicknameExists(char *nickname);

#endif
