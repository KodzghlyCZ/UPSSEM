#include <stdint.h>
#include <stdio.h>
#include <limits.h>
#include <errno.h>

#include "message.h"
#include "server.h"
#include "stringop.h"
#include "commands.h"
#include "parser.h"
#include "stateMachine.h"
#include "game.h"

#define isCommand(s) !strcmp(parts[0], s)	//Makro po porovnani retezce prikazu
uint8_t parseMessage(char *message, client * client)
{
	printf("%s <-- %d\n", message, client->socket);

	if(message[0]==10)
	{
		return 0;
	}
	
	if (strlen(message) != MESSAGE_SIZE) {
		return 1;
	}

	char parts[MESSAGE_PARTS][MESSAGE_SIZE];

	for (int i = 0; i < MESSAGE_PARTS; i++) {
		strncpy(parts[i], message + (i * MESSAGE_PART_SIZE),
			MESSAGE_PART_SIZE);
		parts[i][MESSAGE_PART_SIZE] = '\0';
	}

	replaceChar(parts[0], ' ', 0);
	replaceChar(parts[1], ' ', 0);
	replaceChar(parts[2], ' ', 0);

	if (isCommand(COMMAND_CONNECT)) {
		sendOk(client);
		if (changeState(client, CONNECTING)) {
			return 1;
		}
		return 0;
	}
	if (isCommand(COMMAND_SEND_NICKNAME)) {
		sendOk(client);
		if (client->state == CONNECTING) {
			if (changeState(client, CONNECTING)) {
				return 1;
			}
			strncpy(client->nickname, parts[1],
				MAX_NICKNAME_LENGTH);
			if (changeState(client, CONNECTED)) {
				return 3;
			}
			if (!movePlayerToConnected(client)) {
				unpauseGame(client);
				sendGameCommand(client);
				updatePlayerBet(client);
				updatePlayerCredit(client);
				updatePlayerCards(client);
				broadcastPlayerReconnected(client);
				iterateAvailableGames(addLobbyRoom, client);
				
				if(client->state == GAME_READY)
				{
					sendShowBetUI(client);
				}
				printGame(client->gameId);
			}
			else
			{
				sendLobbyCommand(client);
				if (changeState(client, LOBBY)) {
					return 4;
				}
				printf("Has game space %d\n", hasSpace(0));
				
				iterateAvailableGames(addLobbyRoom, client);
			}
			return 0;
		}
		return 6;
	}
	if (isCommand(COMMAND_JOIN_ROOM)) {
		sendOk(client);

		char *endptr;
		long number = strtol(parts[1], &endptr, 10);

		if (endptr == parts[1]) {
			printf("Invalid input\n");
			return 2;
		} else if (errno == ERANGE || number > INT_MAX
			   || number < INT_MIN) {
			printf("Number out of range\n");
			return 3;
		}
		
		if((int)number>=MAX_GAMES || (int)number<0)
		{
			return 4;
		}
		
		if (changeState(client, GAME_NOT_READY)) {
			return 1;
		}
		
		if (addPlayer((int)number, client)) {
			return 5;
		}
		client->gameId = (int)number;

		broadcastNotReady(client);
		sendPlayerStates(client);
		sendShowReadyButton(client);
		sendClearCards(client);
		sendGameCommand(client);
		printf("Added player to room %d\n", (int)number);

		return 0;
	}
	if (isCommand(COMMAND_GAME_READY)) {
		sendOk(client);
		if (changeState(client, GAME_READY)) {
			return 1;
		}
		broadcastReady(client);
		printf("Player ready\n");
		if (!checkPlayersReady(client->gameId)) {
			startGame(client->gameId);
		}
		return 0;
	}

	if (isCommand(COMMAND_PLACE_BET)) {
		sendOk(client);
		printf("Bet placing by %s\n", client->nickname);
		if (isGameRunning(client->gameId)) {
			return 1;
		}
		
		char *endptr;
		long number = strtol(parts[1], &endptr, 10);

		if (endptr == parts[1]) {
			printf("Invalid input\n");
			return 3;
		} else if (errno == ERANGE || number > INT_MAX
			   || number < INT_MIN) {
			printf("Number out of range\n");
			return 4;
		}

		if ((int)number > client->credit || (int)number<0) {
			return 5;
		}
		
		if (changeState(client, GAME_PLACED_BET)) {
			return 2;
		}
		
		client->currBet = (int)number;
		client->credit -= (int)number;

		updatePlayerBet(client);
		updatePlayerCredit(client);

		if (!checkBetsPlaced(client->gameId)) {
			newRound(client->gameId);
			printGame(client->gameId);
			//checkInitialWin(client->gameId);
		}

		printf("Bet placed by %s\n", client->nickname);

		return 0;
	}
	if (isCommand(COMMAND_HIT)) {
		sendOk(client);
		if (!isPaused(client->gameId)) {
			return 1;
		}
		if (client->state != GAME_TURN) {
			return 2;
		}
		gameHit(client);
		if (!checkRoundComplete(client->gameId)) {
			endRound(client->gameId);
		} else {
			nextTurn(client->gameId);
		}
		printGame(client->gameId);

		return 0;
	}
	if (isCommand(COMMAND_STAY)) {
		sendOk(client);
		if (!isPaused(client->gameId)) {
			return 1;
		}
		if (client->state != GAME_TURN) {
			return 2;
		}
		gameStay(client);
		printGame(client->gameId);
		if (!checkRoundComplete(client->gameId)) {
			endRound(client->gameId);
		} else {
			nextTurn(client->gameId);
		}

		return 0;
	}
	if (isCommand(COMMAND_LEAVE)) {
		sendOk(client);
		if (!(client->state == GAME_TURN ||
		      client->state == GAME_WAITING_NEW_ROUND ||
		      client->state == GAME_WAITING ||
		      client->state == GAME_WAITING_NEW_ROUND)
		    ) {
			return 1;
		}
		if (isPaused(client->gameId)) {
			return 2;
		}

		endGame(client->gameId, 1);

		return 0;
	}
	if (isCommand(COMMAND_OK)) {
		return 0;
	}
	if (isCommand("\n")) {
		return 0;
	}
	return 1;
}
