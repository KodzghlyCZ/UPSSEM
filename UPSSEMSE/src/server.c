#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/socket.h>

#include <string.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <unistd.h>

#include "message.h"
#include "client.h"
#include "signals.h"
#include "parser.h"
#include "stringop.h"
#include "game.h"
#include "queue.h"

#include "server.h"

int listenSock;
client connectionList[MAX_CLIENTS];

void shutdownProperly(int code)
{
	close(listenSock);

	for (int i = 0; i < MAX_CLIENTS; ++i)
		if (connectionList[i].socket != NO_SOCKET)
			close(connectionList[i].socket);

	printf("Shutdown server properly.\n");
	exit(code);
}

/* Start listening socket listenSock. */
int startListenSocket(int *listenSock, char *ip, int port)
{
	// Obtain a file descriptor for our "listening" socket.
	*listenSock = socket(AF_INET, SOCK_STREAM, 0);
	if (*listenSock < 0) {
		perror("socket");
		return -1;
	}

	int reuse = 1;
	if (setsockopt
	    (*listenSock, SOL_SOCKET, SO_REUSEADDR, &reuse,
	     sizeof(reuse)) != 0) {
		perror("setsockopt");
		return -1;
	}

	struct sockaddr_in myAddr;
	memset(&myAddr, 0, sizeof(myAddr));
	myAddr.sin_family = AF_INET;
	myAddr.sin_addr.s_addr = inet_addr(ip);
	myAddr.sin_port = htons(port);

	if (bind
	    (*listenSock, (struct sockaddr *)&myAddr,
	     sizeof(struct sockaddr)) != 0) {
		perror("bind");
		return -1;
	}
	// start accept client connections
	if (listen(*listenSock, 10) != 0) {
		perror("listen");
		return -1;
	}
	printf("Accepting connections on port %d.\n", (int)port);

	return 0;
}

int buildFdSets(fd_set * readFds, fd_set * writeFds, fd_set * exceptFds)
{
	FD_ZERO(readFds);
	FD_SET(STDIN_FILENO, readFds);
	FD_SET(listenSock, readFds);
	for (int i = 0; i < MAX_CLIENTS; ++i)
		if (connectionList[i].socket != NO_SOCKET)
			FD_SET(connectionList[i].socket, readFds);

	FD_ZERO(writeFds);
	for (int i = 0; i < MAX_CLIENTS; ++i) {
		if (connectionList[i].socket != NO_SOCKET
		    && connectionList[i].sendBuffer.current > 0) {
			FD_SET(connectionList[i].socket, writeFds);
		}
	}

	FD_ZERO(exceptFds);
	FD_SET(STDIN_FILENO, exceptFds);
	FD_SET(listenSock, exceptFds);
	for (int i = 0; i < MAX_CLIENTS; ++i)
		if (connectionList[i].socket != NO_SOCKET)
			FD_SET(connectionList[i].socket, exceptFds);

	return 0;
}

int handleNewConnection()
{
	struct sockaddr_in clientAddr;
	memset(&clientAddr, 0, sizeof(clientAddr));
	socklen_t clientLen = sizeof(clientAddr);
	int newClientSock =
	    accept(listenSock, (struct sockaddr *)&clientAddr, &clientLen);
	if (newClientSock < 0) {
		perror("accept()");
		return -1;
	}

	char clientIpv4Str[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &clientAddr.sin_addr, clientIpv4Str,
		  INET_ADDRSTRLEN);

	printf("Incoming connection from %s:%d.\n", clientIpv4Str,
	       clientAddr.sin_port);

	for (int i = 0; i < MAX_CLIENTS; ++i) {
		if (connectionList[i].socket == NO_SOCKET) {
			connectionList[i].socket = newClientSock;
			connectionList[i].addres = clientAddr;
			connectionList[i].currentSendingByte = -1;
			connectionList[i].currentReceivingByte = 0;
			return 0;
		}
	}

	printf("There is too much connections. Close new connection %s:%d.\n",
	       clientIpv4Str, clientAddr.sin_port);
	close(newClientSock);
	return -1;
}

int closeClientConnection(client * client)
{
	printf("Close client socket for %s.\n", peerGetAddressStr(client));
	broadcastPlayerDisconnected(client);
	pauseGame(client);
	suspendPlayer(client);
	close(client->socket);
	client->socket = NO_SOCKET;
	dequeueAll(&client->sendBuffer);
	client->currentSendingByte = -1;
	client->currentReceivingByte = 0;
	client->nonsense = 0;
	client->state = INACTIVE;
	memset(client->nickname, 0, MAX_NICKNAME_LENGTH);
	for (int i = 0; i < MAX_CARDS_HAND; i++) {
		client->cards[i].value = 0;
	}

	return 0;
}

uint8_t sendMessage(char *messageType, char *messageContent, char *messageTail,
		    client * client)
{
	char buffer[MESSAGE_SIZE];
	concatMessage(messageType, messageContent, messageTail, buffer);

	message_t newMessage;
	prepareMessage(buffer, &newMessage);

	if (client->socket != NO_SOCKET) {
		if (peerAddToSend(client, &newMessage) != 0) {
			printf
			    ("Send buffer was overflowed, we lost this message!\n");
			return 1;
		}
		//printf("New message to send was enqueued right now.\n");
		return 0;
	}
	return 2;
}

uint8_t broadcastMessage(char *messageType, char *messageContent,
			 char *messageTail)
{
	for (int i = 0; i < MAX_CLIENTS; ++i) {
		sendMessage(messageType, messageContent, messageTail,
			    &connectionList[i]);
	}
	return 0;
}

int main(int argc, char **argv)
{
	if (argc != 3) {
		printf("Usage: %s <IP> <PORT>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	if (setupSignals() != 0)
		exit(EXIT_FAILURE);

	if (startListenSocket(&listenSock, argv[1], atoi(argv[2])) != 0)
		exit(EXIT_FAILURE);

	int i;
	for (i = 0; i < MAX_CLIENTS; ++i) {
		connectionList[i].socket = NO_SOCKET;
		createClient(&connectionList[i]);
	}

	initGames();

	fd_set readFds;
	fd_set writeFds;
	fd_set exceptFds;

	int highSock = listenSock;

	printf("Waiting for incoming connections.\n");

	for (;;) {
		buildFdSets(&readFds, &writeFds, &exceptFds);

		highSock = listenSock;
		for (i = 0; i < MAX_CLIENTS; i++) {
			if (connectionList[i].socket > highSock)
				highSock = connectionList[i].socket;
		}

		int activity =
		    select(highSock + 1, &readFds, &writeFds, &exceptFds, NULL);

		switch (activity) {
		case -1:
			perror("select()");
			shutdownProperly(EXIT_FAILURE);
			break;

		case 0:
			// you should never get here
			printf("select() returns 0.\n");
			shutdownProperly(EXIT_FAILURE);
			break;

		default:
			if (FD_ISSET(listenSock, &readFds)) {
				handleNewConnection();
			}

			if (FD_ISSET(STDIN_FILENO, &exceptFds)) {
				printf("exceptFds for stdin.\n");
				shutdownProperly(EXIT_FAILURE);
			}

			if (FD_ISSET(listenSock, &exceptFds)) {
				printf("Exception listen socket fd.\n");
				shutdownProperly(EXIT_FAILURE);
			}

			for (int i = 0; i < MAX_CLIENTS; ++i) {
				if (connectionList[i].socket != NO_SOCKET
				    && FD_ISSET(connectionList[i].socket,
						&readFds)) {
					if (receiveFromClient
					    (&connectionList[i]) != 0) {
						closeClientConnection
						    (&connectionList[i]);
						continue;
					}
				}

				if (connectionList[i].socket != NO_SOCKET
				    && FD_ISSET(connectionList[i].socket,
						&writeFds)) {
					if (sendToClient(&connectionList[i]) !=
					    0) {
						closeClientConnection
						    (&connectionList[i]);
						continue;
					}
				}

				if (connectionList[i].socket != NO_SOCKET
				    && FD_ISSET(connectionList[i].socket,
						&exceptFds)) {
					printf("Exception client fd.\n");
					closeClientConnection(&connectionList
							      [i]);
					continue;
				}
			}
		}
	}

	return 0;
}

int receiveFromClient(client * peer)
{
	ssize_t receivedCount;
	char receivedMessage[MESSAGE_SIZE + 1];
	memset(receivedMessage, 0, MESSAGE_SIZE + 1);
	receivedCount =
	    recv(peer->socket, receivedMessage, MESSAGE_SIZE, MSG_DONTWAIT);
	if (receivedCount < 0) {
		if (errno == EAGAIN || errno == EWOULDBLOCK) {
			printf
			    ("peer is not ready right now, try again later.\n");
		} else {
			perror("recv() from peer error");
			return -1;
		}
	} else if (receivedCount < 0
		   && (errno == EAGAIN || errno == EWOULDBLOCK)) {
		return 1;
	}
	// If recv() returns 0, it means that peer gracefully shutdown. Shutdown client.
	else if (receivedCount == 0) {
		printf("recv() 0 bytes. Peer gracefully shutdown.\n");
		return -1;
	} else if (receivedCount > 0) {
		int retCode = parseMessage(receivedMessage, peer);
		if (retCode) {
			printf("Something sussy hapenning with error of %d\n",
			       retCode);
			printf("Nonsense @ %ld\n", peer->nonsense);
			if (++peer->nonsense >= NONSENSE_THRESHOLD) {
				closeClientConnection(peer);
			}
		}
	}
	return 0;
}

int sendToClient(client * peer)
{
	size_t lenToSend;
	ssize_t sendCount;
	size_t sendTotal = 0;
	do {
		if (peer->currentSendingByte >= sizeof(peer->sendingBuffer)) {
			//printf("There is no pending to send() message, maybe we can find one in queue... ");
			if (dequeue(&peer->sendBuffer, &peer->sendingBuffer) !=
			    0) {
				peer->currentSendingByte = -1;
				//printf("No, there is nothing to send() anymore.\n");
				break;
			}
			//printf("Yes, pop and send() one of them.\n");
			peer->currentSendingByte = 0;
		}
		// Count bytes to send.
		lenToSend =
		    sizeof(peer->sendingBuffer) - peer->currentSendingByte;
		if (lenToSend > MESSAGE_SIZE)
			lenToSend = MESSAGE_SIZE;

		//printf("Let's try to send() %zd bytes... ", lenToSend);
		printf("%s --> %d\n", peer->sendingBuffer.data, peer->socket);
		sendCount =
		    send(peer->socket,
			 (char *)&peer->sendingBuffer +
			 peer->currentSendingByte, lenToSend, 0);
		if (sendCount < 0) {
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				printf
				    ("peer is not ready right now, try again later.\n");
			} else {
				perror("send() from peer error");
				return -1;
			}
		}
		// we have read as many as possible
		else if (sendCount < 0
			 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
			break;
		} else if (sendCount == 0) {
			printf
			    ("send()'ed 0 bytes. It seems that peer can't accept data right now. Try again later.\n");
			break;
		} else if (sendCount > 0) {
			peer->currentSendingByte += sendCount;
			sendTotal += sendCount;
			//printf("send()'ed %zd bytes.\n", sendCount);
		}
	} while (sendCount > 0);

	//printf("Total send()'ed %zu bytes.\n", sendTotal);
	return 0;
}

uint8_t checkNicknameExists(char *nickname)
{
	for (int i = 0; i < MAX_CLIENTS; i++) {
		if (!strncmp
		    (connectionList[i].nickname, nickname, strlen(nickname))) {
			return 1;
		}
	}
	return 0;
}
