#include <time.h>

#include "commands.h"
#include "stateMachine.h"
#include "card.h"

#include "game.h"

static game games[MAX_GAMES];

uint8_t endRound(int gameId)
{
	printf("Ending round\n");
	if (games[gameId].dealerCardsValue < 17) {
		dealDealerCard(gameId);
	}

	if (games[gameId].dealerCardsValue > 21) {
		for (int i = 0; i < MAX_PLAYERS_IN_GAME; i++) {
			if (games[gameId].players[i] != NULL && games[gameId].players[i]->cardValue<21) {
				sendDealerBusts(games[gameId].players[i]);
				sendWin(games[gameId].players[i]);
				games[gameId].players[i]->credit +=
				    (games[gameId].players[i]->currBet * 2);
			}
		}
	} else {
		for (int i = 0; i < MAX_PLAYERS_IN_GAME; i++) {
			if (games[gameId].players[i] != NULL) {
				if (games[gameId].players[i]->cardValue > games[gameId].dealerCardsValue && 
					games[gameId].players[i]->cardValue < 21) {
					sendHigherThanDealer(games[gameId].players[i]);
					sendWin(games[gameId].players[i]);
					games[gameId].players[i]->credit +=
					    (games[gameId].players[i]->currBet * 2);
				} else {
					sendLose(games[gameId].players[i]);
				}
			}
		}
	}
	for (int i = 0; i < MAX_PLAYERS_IN_GAME; i++) {
		if (games[gameId].players[i] != NULL) {
			games[gameId].players[i]->currBet = 0;
			games[gameId].players[i]->cardValue = 0;
			updatePlayerCredit(games[gameId].players[i]);
			updatePlayerBet(games[gameId].players[i]);

			for (int j = 0; j < MAX_CARDS_HAND; j++) {
				if (games[gameId].players[i]->cards[j].value !=
				    0) {
					games[gameId].players[i]->cards[j].
					    value = 0;
				}
			}
		}
	}
	printf("Ended round\n");

	if (games[gameId].round == GAME_ROUNDS) {
		endGame(gameId, 0);
	} else {
		games[gameId].round++;

		for (int i = 0; i < MAX_PLAYERS_IN_GAME; i++) {
			if (games[gameId].players[i] != NULL) {
				sendClearCards(games[gameId].players[i]);
				changeState(games[gameId].players[i],
					    GAME_READY);
				sendShowBetUI(games[gameId].players[i]);
			}
		}
	}
	return 0;
}

uint8_t endGame(int gameId, int directEnd)
{
	printf("Ending game\n");
	games[gameId].round = 0;
	games[gameId].dealerCardsValue = 0;
	games[gameId].isPaused = 0;
	games[gameId].currPlayer = -1;

	for (int i = 0; i < DECK_CARDS; i++) {
		if (games[gameId].cards[i] != NULL) {
			free(games[gameId].cards[i]);
			games[gameId].cards[i] = NULL;
		}
	}

	for (int i = 0; i < MAX_PLAYERS_IN_GAME; i++) {
		if (games[gameId].players[i] != NULL) {
			if (directEnd) {
				sendLobbyCommand(games[gameId].players[i]);
			} else {
				sendLobbyButton(games[gameId].players[i]);
			}

			games[gameId].players[i]->state = LOBBY;
			games[gameId].players[i] = NULL;
		}
		if (games[gameId].disconnectedPlayers[i] != NULL) {
			free(games[gameId].disconnectedPlayers[i]);
			games[gameId].disconnectedPlayers[i] = NULL;
		}
	}

	printf("Ended game\n");
	printGame(gameId);
	return 0;
}

uint8_t gameHit(client * client)
{
	dealGameCard(client->gameId, client);
	if (client->cardValue > 21) {
		setPlayerWaitingRoundEnd(client);
		client->currBet = 0;
		updatePlayerCredit(client);
		updatePlayerBet(client);
		sendBust(client);
	}
	if (client->cardValue == 21) {
		setPlayerWaitingRoundEnd(client);
		client->credit += client->currBet * 3 / 2;
		client->currBet = 0;
		updatePlayerCredit(client);
		updatePlayerBet(client);
		sendWin(client);
	}

	return 0;
}

uint8_t gameStay(client * client)
{
	setPlayerWaitingRoundEnd(client);

	return 0;
}

uint8_t setPlayerWaitingRoundEnd(client * client)
{
	if (games[client->gameId].round == GAME_ROUNDS) {
		client->state = GAME_WAITING_END;
	} else {
		client->state = GAME_WAITING_NEW_ROUND;
	}
	return 0;
}

uint8_t isGameRunning(int gameId)
{
	return !(games[gameId].round > 0);
}

uint8_t pauseGame(client * client)
{
	if (client->gameId >= 0) {
		games[client->gameId].isPaused = 1;
		for (int i = 0; i < MAX_PLAYERS_IN_GAME; i++) {
			if (games[client->gameId].players[i] != NULL) {
				sendPause(games[client->gameId].players[i]);
			}
		}

		if (hasActivePlayers(client->gameId)) {
			printf("Ending empty game\n");
			endGame(client->gameId, 1);
		}

		return 0;
	}
	return 1;
}

uint8_t hasActivePlayers(int gameId)
{
	for (int i = 0; i < MAX_PLAYERS_IN_GAME; i++) {
		if (games[gameId].players[i] != NULL) {
			return 0;
		}
	}
	return 1;
}

uint8_t unpauseGame(client * client)
{
	if (client->gameId >= 0) {
		printf("Unpausing game\n");
		games[client->gameId].isPaused = 0;
		for (int i = 0; i < MAX_PLAYERS_IN_GAME; i++) {
			if (games[client->gameId].players[i] != NULL) {
				sendUnpause(games[client->gameId].players[i]);
			}
		}
		printf("Unpaused game\n");
		return 0;
	}
	return 1;
}

uint8_t isPaused(int gameId)
{
	return !games[gameId].isPaused;
}

uint8_t initGames()
{
	for (int i = 0; i < MAX_GAMES; i++) {
		games[i].round = 0;
		games[i].isPaused = 0;
		games[i].dealerCardsValue = 0;
	}
	return 0;
}

uint8_t iterateAvailableGames(uint8_t(*func) (int, client *), client * client)
{
	for (int i = 0; i < MAX_GAMES; i++) {
		if (hasSpace(i) && func(i, client)) {
			return 1;
		}
	}
	return 0;
}

uint8_t hasSpace(int gameId)
{
	int playerCount = 0;
	int disconnectedPlayerCount = 0;
	for (int i = 0; i < MAX_PLAYERS_IN_GAME; i++) {
		if (games[gameId].players[i] != NULL) {
			playerCount++;
		}
		if (games[gameId].disconnectedPlayers[i] != NULL) {
			disconnectedPlayerCount++;
		}
	}
	return ((playerCount + disconnectedPlayerCount) < MAX_PLAYERS_IN_GAME
		&& playerCount < MAX_PLAYERS_IN_GAME
		&& disconnectedPlayerCount < MAX_PLAYERS_IN_GAME);
}

uint8_t addPlayer(int gameId, client * client)
{
	if(!isGameRunning(gameId))
	{
		return 1;
	}
	for (int i = 0; i < MAX_PLAYERS_IN_GAME; i++) {
		if (games[gameId].players[i] == NULL) {
			games[gameId].players[i] = client;
			return 0;
		}
	}
	return 2;
}

uint8_t initGame(int gameId)
{
	games[gameId].round = 0;
	for (int i = 0; i < MAX_GAMES; i++) {
		games[gameId].players[i] = NULL;
		games[gameId].disconnectedPlayers[i] = NULL;
	}
	return 0;
}

uint8_t checkPlayersReady(int gameId)
{
	for (int i = 0; i < MAX_PLAYERS_IN_GAME; i++) {
		if (games[gameId].players[i] != NULL
		    && games[gameId].players[i]->state == GAME_NOT_READY) {
			return 1;
		}
	}
	return 0;
}

uint8_t checkRoundComplete(int gameId)
{
	printf("Checking round complete\n");
	for (int i = 0; i < MAX_PLAYERS_IN_GAME; i++) {
		if (games[gameId].players[i] != NULL
		    && !(games[gameId].players[i]->state ==
			 GAME_WAITING_NEW_ROUND
			 || games[gameId].players[i]->state ==
			 GAME_WAITING_END)) {
			printf("Round is not complete\n");
			return 1;
		}
	}
	printf("Round is complete\n");
	return 0;
}

uint8_t startGame(int gameId)
{
	games[gameId].round = 1;
	games[gameId].dealerCardsValue = 0;
	for (int i = 0; i < MAX_PLAYERS_IN_GAME; i++) {
		if (games[gameId].players[i] != NULL) {
			games[gameId].players[i]->credit =
			    DEFAULT_PLAYER_CREDIT;
			sendClearCards(games[gameId].players[i]);
			updatePlayerCredit(games[gameId].players[i]);
			sendShowBetUI(games[gameId].players[i]);
		}
	}

	return 1;
}

uint8_t dealGameCard(int gameId, client * client)
{
	srand(time(NULL));
	int r = rand() % DECK_CARDS;
	while (games[gameId].cards[r]->value == 0) {
		r = rand() % DECK_CARDS;
	}
	dealCard(games[gameId].cards[r], client);
	games[gameId].cards[r]->value = 0;
	return 0;
}

uint8_t dealDealerCard(int gameId)
{
	srand(time(NULL));
	int r = rand() % DECK_CARDS;
	while (games[gameId].cards[r]->value == 0) {
		r = rand() % DECK_CARDS;
	}

	games[gameId].dealerCardsValue += games[gameId].cards[r]->value;
	printf
	    ("Dealt dealer %s card with value of %d, dealer has %d in total.\n",
	     games[gameId].cards[r]->name, games[gameId].cards[r]->value,
	     games[gameId].dealerCardsValue);
	games[gameId].cards[r]->value = 0;

	return 0;
}

uint8_t newRound(int gameId)
{
	printf("Beggining new round\n");
	games[gameId].dealerCardsValue = 0;
	card cards[DECK_CARDS] = {
		{ "cA", 1 }, { "c2", 2 }, { "c3", 3 }, { "c4", 4 }, { "c5",
								      5 },
		    { "c6", 6 }, { "c7", 7 },
		{ "c8", 8 }, { "c9", 9 }, { "cX", 10 }, { "cJ", 10 }, { "cQ",
								       10 },
		    { "cK", 10 },
		{ "dA", 1 }, { "d2", 2 }, { "d3", 3 }, { "d4", 4 }, { "d5",
								      5 },
		    { "d6", 6 }, { "d7", 7 },
		{ "d8", 8 }, { "d9", 9 }, { "dX", 10 }, { "dJ", 10 }, { "dQ",
								       10 },
		    { "dK", 10 },
		{ "hA", 1 }, { "h2", 2 }, { "h3", 3 }, { "h4", 4 }, { "h5",
								      5 },
		    { "h6", 6 }, { "h7", 7 },
		{ "h8", 8 }, { "h9", 9 }, { "hX", 10 }, { "hJ", 10 }, { "hQ",
								       10 },
		    { "hK", 10 },
		{ "sA", 1 }, { "s2", 2 }, { "s3", 3 }, { "s4", 4 }, { "s5",
								      5 },
		    { "s6", 6 }, { "s7", 7 },
		{ "s8", 8 }, { "s9", 9 }, { "sX", 10 }, { "sJ", 10 }, { "sQ",
								       10 },
		    { "sK", 10 },
	};
	for (int i = 0; i < DECK_CARDS; i++) {
		if (games[gameId].cards[i] != NULL) {
			free(games[gameId].cards[i]);
			games[gameId].cards[i] = NULL;
		}
		games[gameId].cards[i] = calloc(1, sizeof(card));
		games[gameId].cards[i]->value = cards[i].value;
		strncpy(games[gameId].cards[i]->name, cards[i].name,
			CARD_NAME_LENGTH);
	}

	for (int i = 0; i < MAX_PLAYERS_IN_GAME; i++) {
		if (games[gameId].players[i] != NULL) {
			changeState(games[gameId].players[i], GAME_WAITING);
			dealGameCard(gameId, games[gameId].players[i]);
			dealGameCard(gameId, games[gameId].players[i]);
		}
	}
	dealDealerCard(gameId);
	dealDealerCard(gameId);

	for (int i = 0; i < MAX_PLAYERS_IN_GAME; i++) {
		if (games[gameId].players[i] != NULL) {
			games[gameId].currPlayer = i;
			changeState(games[gameId].players[i], GAME_TURN);

			broadcastPlayerTurns(games[gameId].players[i]);

			return 0;
		}
	}
	printf("Began new round\n");
	return 1;
}

uint8_t nextTurn(int gameId)
{
	printf("Looking for next turn\n");
	if (games[gameId].players[games[gameId].currPlayer]->state == GAME_TURN) {
		printf("%s is still turning, switching to waiting\n",
		       games[gameId].players[games[gameId].currPlayer]->
		       nickname);
		changeState(games[gameId].players[games[gameId].currPlayer],
			    GAME_WAITING);
	}

	for (size_t i = games[gameId].currPlayer + 1;
	     i <= MAX_PLAYERS_IN_GAME + games[gameId].currPlayer; i++) {
		if (games[gameId].players[i % MAX_PLAYERS_IN_GAME] != NULL
		    && games[gameId].players[i % MAX_PLAYERS_IN_GAME]->state ==
		    GAME_WAITING) {
			games[gameId].currPlayer = i % MAX_PLAYERS_IN_GAME;
			changeState(games[gameId].
				    players[i % MAX_PLAYERS_IN_GAME],
				    GAME_TURN);
			printf("%s is the next player\n",
			       games[gameId].players[i %
						     MAX_PLAYERS_IN_GAME]->
			       nickname);

			broadcastPlayerTurns(games[gameId].
					     players[i % MAX_PLAYERS_IN_GAME]);

			return 0;
		}
	}
	return 1;
}

/*uint8_t checkInitialWin(int gameId)
{
	printf("Checking initial win\n");
	for (int i = 0; i < MAX_PLAYERS_IN_GAME; i++) {
		if (games[gameId].players[i] != NULL) {
			printf("Player %d has cards of value %d\n", i,
			       games[gameId].players[i]->cardValue);
			if (games[gameId].players[i]->cardValue == 21) {
				printf
				    ("Player has 21, waiting for new round\n");

				if (games[gameId].players[i]->state ==
				    GAME_TURN) {
					games[gameId].players[i]->state =
					    GAME_WAITING_NEW_ROUND;
					nextTurn(gameId);
				}
				games[gameId].players[i]->credit +=
				    (games[gameId].players[i]->currBet * 3 / 2);
			}
		}
	}
	printf("Checked initial win\n");
	return 0;
}*/

uint8_t checkAllWaitingNewRound(int gameId)
{
	printf("Checking for all players waiting for a new round\n");
	for (int i = 0; i < MAX_PLAYERS_IN_GAME; i++) {
		if (games[gameId].players[i] != NULL
		    && games[gameId].players[i]->state !=
		    GAME_WAITING_NEW_ROUND) {
			printf("Not all players are waiting for a new round\n");
			return 1;
		}
	}
	printf("Checked for all players waiting for a new round\n");
	return 0;
}

uint8_t dealCard(card * card, client * client)
{
	printf("Dealing card %s to %s\n", card->name, client->nickname);
	client->cardValue += card->value;

	for (int i = 0; i < MAX_CARDS_HAND; i++) {
		if (client->cards[i].value == 0) {
			client->cards[i].value = card->value;
			strncpy(client->cards[i].name, card->name,
				CARD_NAME_LENGTH);
			sendDealCard(client, card);
			return 0;
			printf("Dealt card %s to %s\n", card->name,
			       client->nickname);
		}
	}
	printf("Error dealing card %s to %s\n", card->name, client->nickname);
	return 1;
}

uint8_t broadcastNotReady(client * client)
{
	for (int i = 0; i < MAX_PLAYERS_IN_GAME; i++) {
		if (games[client->gameId].players[i] != NULL
		    && games[client->gameId].players[i] != client) {
			sendPlayerNotReady(games[client->gameId].players[i],
					   client->nickname);
		}
	}
	return 0;
}

uint8_t broadcastReady(client * client)
{
	for (int i = 0; i < MAX_PLAYERS_IN_GAME; i++) {
		if (games[client->gameId].players[i] != NULL
		    && games[client->gameId].players[i] != client) {
			sendPlayerReady(games[client->gameId].players[i],
					client->nickname);
		}
	}
	return 0;
}

uint8_t sendPlayerStates(client * client)
{
	for (int i = 0; i < MAX_PLAYERS_IN_GAME; i++) {
		if (games[client->gameId].players[i] != NULL
		    && games[client->gameId].players[i] != client) {
			if (games[client->gameId].players[i]->state ==
			    GAME_NOT_READY) {
				sendPlayerNotReady(client,
						   games[client->gameId].
						   players[i]->nickname);
			}
			if (games[client->gameId].players[i]->state ==
			    GAME_READY) {
				sendPlayerReady(client,
						games[client->gameId].
						players[i]->nickname);
			}
		}
	}
	return 0;
}

uint8_t suspendPlayer(client * client)
{
	for (int i = 0; i < MAX_PLAYERS_IN_GAME; i++) {
		if (games[client->gameId].players[i] == client) {
			if (games[client->gameId].round > 0) {
				movePlayerToDisconnected(client);
			} else {
				games[client->gameId].players[i] = NULL;
			}
		}
	}
	return 0;
}

uint8_t movePlayerToDisconnected(client * client)
{
	printf("Moving player to suspended\n");
	printGame(client->gameId);
	for (int i = 0; i < MAX_PLAYERS_IN_GAME; i++) {
		if (games[client->gameId].players[i] == client) {
			for (int j = 0; j < MAX_PLAYERS_IN_GAME; j++) {
				if (games[client->gameId].
				    disconnectedPlayers[j] == NULL) {
					games[client->gameId].
					    disconnectedPlayers[j] =
					    calloc(MAX_PLAYERS_IN_GAME,
						   sizeof(disconnectedClient));
					games[client->gameId].
					    disconnectedPlayers[j]->gameId =
					    games[client->gameId].players[i]->
					    gameId;
					games[client->gameId].
					    disconnectedPlayers[j]->credit =
					    games[client->gameId].players[i]->
					    credit;
					games[client->gameId].
					    disconnectedPlayers[j]->currBet =
					    games[client->gameId].players[i]->
					    currBet;
					games[client->gameId].
					    disconnectedPlayers[j]->state =
					    games[client->gameId].players[i]->
					    state;
					games[client->gameId].
					    disconnectedPlayers[j]->cardValue =
					    games[client->gameId].players[i]->
					    cardValue;
					strncpy(games[client->gameId].
						disconnectedPlayers[j]->
						nickname,
						games[client->gameId].
						players[i]->nickname,
						MAX_NICKNAME_LENGTH);

					for (int k = 0; k < MAX_CARDS_HAND; k++) {
						if (games[client->gameId].
						    players[i]->cards[k].
						    value != 0) {
							games[client->gameId].
							    disconnectedPlayers
							    [j]->cards[k].
							    value =
							    games[client->
								  gameId].
							    players[i]->
							    cards[k].value;
							strncpy(games
								[client->
								 gameId].
								disconnectedPlayers
								[j]->cards[k].
								name,
								games[client->
								      gameId].
								players[i]->
								cards[k].name,
								CARD_NAME_LENGTH);
						}
					}

					games[client->gameId].players[i] = NULL;

					printf("Suspended player: \n");
					printf("gameId: %d\n",
					       games[client->gameId].
					       disconnectedPlayers[j]->gameId);
					printf("credit: %d\n",
					       games[client->gameId].
					       disconnectedPlayers[j]->credit);
					printf("bet: %d\n",
					       games[client->gameId].
					       disconnectedPlayers[j]->currBet);
					printf("nickname: %s\n",
					       games[client->gameId].
					       disconnectedPlayers[j]->
					       nickname);

					return 0;
				}
			}
		}
	}
	return 1;
}

uint8_t movePlayerToConnected(client * client)
{
	printf("Restoring player\n");
	for (int i = 0; i < MAX_GAMES; i++) {
		for (int j = 0; j < MAX_PLAYERS_IN_GAME; j++) {
			if (games[i].disconnectedPlayers[j] != NULL) {
				printf
				    ("Found record with name of %s, reconnecting client name is %s\n",
				     games[i].disconnectedPlayers[j]->nickname,
				     client->nickname);
			}
			if (games[i].disconnectedPlayers[j] != NULL &&
			    !strncmp(games[i].disconnectedPlayers[j]->nickname,
				     client->nickname, MAX_NICKNAME_LENGTH)) {
				for (int k = 0; k < MAX_PLAYERS_IN_GAME; k++) {
					if (games[i].players[k] == NULL) {
						client->gameId =
						    games[i].
						    disconnectedPlayers[j]->
						    gameId;
						client->credit =
						    games[i].
						    disconnectedPlayers[j]->
						    credit;
						client->currBet =
						    games[i].
						    disconnectedPlayers[j]->
						    currBet;
						client->state =
						    games[i].
						    disconnectedPlayers[j]->
						    state;
						client->cardValue =
						    games[i].
						    disconnectedPlayers[j]->
						    cardValue;

						for (int l = 0;
						     l < MAX_CARDS_HAND; l++) {
							if (games[i].
							    disconnectedPlayers
							    [j]->cards[l].
							    value != 0) {
								client->
								    cards[l].
								    value =
								    games[i].
								    disconnectedPlayers
								    [j]->
								    cards[l].
								    value;
								strncpy(client->
									cards
									[l].
									name,
									games
									[i].
									disconnectedPlayers
									[j]->
									cards
									[l].
									name,
									CARD_NAME_LENGTH);
							}
						}

						free(games[i].
						     disconnectedPlayers[j]);
						games[i].
						    disconnectedPlayers[j] =
						    NULL;
						games[i].players[k] = client;

						printf("Restored player\n");
						printGame(client->gameId);
						return 0;
					}
				}
			}
		}
	}
	return 1;
}

uint8_t printGame(int gameId)
{
	printf("ID: %d\n", gameId);
	printf("Round: %lu\n", games[gameId].round);
	printf("Paused: %d\n", games[gameId].isPaused);
	if (games[gameId].players[games[gameId].currPlayer] != NULL) {
		printf("Current player: %s\n",
		       games[gameId].players[games[gameId].currPlayer]->
		       nickname);
	} else {
		printf("Current player: None\n");
	}

	printf("Connected players:\n");
	for (int i = 0; i < MAX_PLAYERS_IN_GAME; i++) {
		if (games[gameId].players[i] != NULL) {
			printf
			    ("#%d: %s, Credit: %d, Bet: %d, State: %d, Card Value: %d, Game ID: %d\n",
			     i, games[gameId].players[i]->nickname,
			     games[gameId].players[i]->credit,
			     games[gameId].players[i]->currBet,
			     games[gameId].players[i]->state,
			     games[gameId].players[i]->cardValue,
			     games[gameId].players[i]->gameId);

			printf("Players cards:\n");
			for (int j = 0; j < MAX_CARDS_HAND; j++) {
				if (games[gameId].players[i]->cards[j].value !=
				    0) {
					printf("Card %s : %d\n",
					       games[gameId].players[i]->
					       cards[j].name,
					       games[gameId].players[i]->
					       cards[j].value);
				}
			}
		}
	}
	printf("Disconnected players:\n");
	for (int i = 0; i < MAX_PLAYERS_IN_GAME; i++) {
		if (games[gameId].disconnectedPlayers[i] != NULL) {
			printf
			    ("#%d: %s, Credit: %d, Bet: %d, State: %d, Card Value: %d, Game ID: %d\n",
			     i, games[gameId].disconnectedPlayers[i]->nickname,
			     games[gameId].disconnectedPlayers[i]->credit,
			     games[gameId].disconnectedPlayers[i]->currBet,
			     games[gameId].disconnectedPlayers[i]->state,
			     games[gameId].disconnectedPlayers[i]->cardValue,
			     games[gameId].disconnectedPlayers[i]->gameId);

			printf("Players cards:\n");
			for (int j = 0; j < MAX_CARDS_HAND; j++) {
				if (games[gameId].disconnectedPlayers[i]->
				    cards[j].value != 0) {
					printf("Card %s : %d\n",
					       games[gameId].
					       disconnectedPlayers[i]->cards[j].
					       name,
					       games[gameId].
					       disconnectedPlayers[i]->cards[j].
					       value);
				}
			}
		}
	}
	return 0;
}

uint8_t checkBetsPlaced(int gameId)
{
	printf("Checking all bets placed\n");
	for (int i = 0; i < MAX_PLAYERS_IN_GAME; i++) {
		if (games[gameId].players[i] != NULL
		    && games[gameId].players[i]->state != GAME_PLACED_BET) {
			printf("Not all bets were placed\n");
			return 1;
		}
	}
	return 0;
}

uint8_t broadcastPlayerDisconnected(client * client)
{
	if (client->gameId != -1) {
		for (int i = 0; i < MAX_PLAYERS_IN_GAME; i++) {
			if (games[client->gameId].players[i] != NULL
			    && games[client->gameId].players[i] != client) {
				sendPlayerDisconnected(games[client->gameId].
						       players[i],
						       client->nickname);
			}
		}
		return 0;
	}
	return 1;
}

uint8_t broadcastPlayerReconnected(client * client)
{
	if (client->gameId != -1) {
		for (int i = 0; i < MAX_PLAYERS_IN_GAME; i++) {
			if (games[client->gameId].players[i] != NULL
			    && games[client->gameId].players[i] != client) {
				sendPlayerReconnected(games[client->gameId].
						      players[i],
						      client->nickname);
			}
		}
		return 0;
	}
	return 1;
}

uint8_t broadcastPlayerTurns(client * client)
{
	if (client->gameId != -1) {
		for (int j = 0; j < MAX_PLAYERS_IN_GAME; j++) {
			if (games[client->gameId].players[j] != NULL) {
				if (games[client->gameId].players[j] == client) {
					sendYouTurn(client);
				} else {
					sendPlayerTurns(games[client->gameId].
							players[j],
							client->nickname);
				}
			}
		}
		return 0;
	}
	return 1;
}
