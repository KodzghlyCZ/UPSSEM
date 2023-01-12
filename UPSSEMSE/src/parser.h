#ifndef PARSER_H_
#define PARSER_H_

#include "client.h"

//Funkce zpracovani zpravy 'message' od klienta 'client'
uint8_t parseMessage(char *message, client *client);

#endif
