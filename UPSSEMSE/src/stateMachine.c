#include <stddef.h>

#include "client.h"

#include "stateMachine.h"

static uint8_t transitionTable[NUM_STATES][NUM_STATES] = {
	{ 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0 },
	{ 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0 },
	{ 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0 },
	{ 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0 },
	{ 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0 },
	{ 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0 },
	{ 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1 },
	{ 1, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0 },
	{ 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1 }
};

uint8_t changeState(client * client, enum state nextState)
{
	printf("Changing from %d to %d\n", client->state, nextState);
	if (transitionTable[client->state][nextState]) {
		printf("Changed from %d to %d\n", client->state, nextState);
		client->state = nextState;
		return 0;
	}
	return 1;
}
