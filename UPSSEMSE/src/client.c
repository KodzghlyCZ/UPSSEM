#include <unistd.h>

#include "client.h"

int deletePeer(client * peer)
{
	close(peer->socket);
	deleteMessageQueue(&peer->sendBuffer);
	return 0;
}

int createClient(client * peer)
{
	createMessageQueue(MAX_MESSAGES_BUFFER_SIZE, &peer->sendBuffer);
	peer->state = INACTIVE;
	peer->currentSendingByte = -1;
	peer->currentReceivingByte = 0;
	peer->cardValue = 0;
	peer->gameId = -1;
	for (int i = 0; i < MAX_CARDS_HAND; i++) {
		peer->cards[i].value = 0;
	}

	memset(peer->nickname, 0, MAX_NICKNAME_LENGTH);
	return 0;
}

char *peerGetAddressStr(client * peer)
{
	static char ret[INET_ADDRSTRLEN + 10];
	char peerIpv4Str[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &peer->addres.sin_addr, peerIpv4Str,
		  INET_ADDRSTRLEN);
	sprintf(ret, "%s:%d", peerIpv4Str, peer->addres.sin_port);

	return ret;
}

int peerAddToSend(client * peer, message_t * message)
{
	return enqueue(&peer->sendBuffer, message);
}
