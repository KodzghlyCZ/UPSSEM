#include <stdint.h>

#include "stringop.h"
#include "server.h"
#include "client.h"
#include "card.h"

#include "commands.h"

uint8_t sendLobbyCommand(client * client)
{
	sendMessage(COMMAND_SET_LOBBY, EMPTY_PART, EMPTY_PART, client);
	return 0;
}

uint8_t sendGameCommand(client * client)
{
	sendMessage(COMMAND_SET_GAME, EMPTY_PART, EMPTY_PART, client);
	return 0;
}

uint8_t addLobbyRoom(int gameId, client * client)
{
	char buf[MESSAGE_PART_SIZE];
	sprintf(buf, "%d", gameId);
	sendMessage(COMMAND_ADD_ROOM, buf, EMPTY_PART, client);
	printf("Adding lobby %d\n", gameId);
	return 0;
}

uint8_t sendPlayerNotReady(client * client, char *nickname)
{
	sendMessage(COMMAND_PLAYER_NOT_READY, nickname, EMPTY_PART, client);
	return 0;
}

uint8_t sendPlayerReady(client * client, char *nickname)
{
	sendMessage(COMMAND_PLAYER_READY, nickname, EMPTY_PART, client);
	return 0;
}

uint8_t updatePlayerCredit(client * client)
{
	char creditString[MESSAGE_PART_SIZE];
	snprintf(creditString, MESSAGE_PART_SIZE, "%d", client->credit);

	sendMessage(COMMAND_UPDATE_CREDIT, creditString, EMPTY_PART, client);
	return 0;
}

uint8_t updatePlayerBet(client * client)
{
	char creditString[MESSAGE_PART_SIZE];
	snprintf(creditString, MESSAGE_PART_SIZE, "%d", client->currBet);

	sendMessage(COMMAND_UPDATE_BET, creditString, EMPTY_PART, client);
	return 0;
}

uint8_t sendDealCard(client * client, card * card)
{
	sendMessage(COMMAND_DEAL_CARD, card->name, EMPTY_PART, client);
	return 0;
}

uint8_t sendClearCards(client * client)
{
	sendMessage(COMMAND_CLEAR_CARDS, EMPTY_PART, EMPTY_PART, client);
	return 0;
}

uint8_t sendPause(client * client)
{
	sendMessage(COMMAND_PAUSE, EMPTY_PART, EMPTY_PART, client);
	return 0;
}

uint8_t sendUnpause(client * client)
{
	sendMessage(COMMAND_UNPAUSE, EMPTY_PART, EMPTY_PART, client);
	return 0;
}

uint8_t sendBust(client * client)
{
	sendMessage(COMMAND_BUST, EMPTY_PART, EMPTY_PART, client);
	return 0;
}

uint8_t sendShowBetUI(client * client)
{
	sendMessage(COMMAND_SHOW_BET_UI, EMPTY_PART, EMPTY_PART, client);
	return 0;
}

uint8_t sendOk(client * client)
{
	sendMessage(COMMAND_OK, EMPTY_PART, EMPTY_PART, client);
	return 0;
}

uint8_t sendLobbyButton(client * client)
{
	sendMessage(COMMAND_LOBBY_BUTTON, EMPTY_PART, EMPTY_PART, client);
	return 0;
}

uint8_t sendShowReadyButton(client * client)
{
	sendMessage(COMMAND_SHOW_READY_BUTTON, EMPTY_PART, EMPTY_PART, client);
	return 0;
}

uint8_t sendPlayerDisconnected(client * client, char *nickname)
{
	sendMessage(COMMAND_DISCONNECTED, nickname, EMPTY_PART, client);
	return 0;
}

uint8_t sendPlayerReconnected(client * client, char *nickname)
{
	sendMessage(COMMAND_RECONNECTED, nickname, EMPTY_PART, client);
	return 0;
}

uint8_t sendPlayerTurns(client * client, char *nickname)
{
	sendMessage(COMMAND_PLAYER_TURNS, nickname, EMPTY_PART, client);
	return 0;
}

uint8_t sendYouTurn(client * client)
{
	sendMessage(COMMAND_YOU_TURN, EMPTY_PART, EMPTY_PART, client);
	return 0;
}

uint8_t sendDealerBusts(client * client)
{
	sendMessage(COMMAND_DEALER_BUSTS, EMPTY_PART, EMPTY_PART, client);
	return 0;
}

uint8_t sendWin(client * client)
{
	sendMessage(COMMAND_WIN, EMPTY_PART, EMPTY_PART, client);
	return 0;
}

uint8_t sendLose(client * client)
{
	sendMessage(COMMAND_LOSE, EMPTY_PART, EMPTY_PART, client);
	return 0;
}

uint8_t sendHigherThanDealer(client * client)
{
	sendMessage(COMMAND_HIGHER_THAN_DEALER, EMPTY_PART, EMPTY_PART, client);
	return 0;
}

uint8_t updatePlayerCards(client * client)
{
	sendClearCards(client);
	for (int i = 0; i < MAX_CARDS_HAND; i++) {
		if (client->cards[i].value != 0) {
			sendDealCard(client, &client->cards[i]);
		}
	}
	char creditString[MESSAGE_PART_SIZE];
	snprintf(creditString, MESSAGE_PART_SIZE, "%d", client->currBet);

	sendMessage(COMMAND_UPDATE_BET, creditString, EMPTY_PART, client);
	return 0;
}
