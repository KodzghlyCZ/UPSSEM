#ifndef STATEMACHINE_H_
#define STATEMACHINE_H_

#include <stdint.h>

#include "client.h"

//Funkce prechodu stavu klienta na zaklade definovane tabulky
uint8_t changeState(client *client, enum state nextState);

#endif
