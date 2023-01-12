#ifndef CLIENT_H_
#define CLIENT_H_

#include "queue.h"
#include "clientStates.h"
#include "card.h"

//Maximalni pocet karet hrace
#define MAX_CARDS_HAND 13

//maximalni delka prezdivky hrace
#define MAX_NICKNAME_LENGTH 8

//Struktura pripojeneho klienta
typedef struct {
	
	int socket; //Cislo socketu 
	struct sockaddr_in addres; //Adresa socketu
	enum state state; //Aktualni stav
	int gameId; //Cislo hry klienta
	int credit; //Aktualni kredit
	int currBet; //Aktualni nabidka
	int cardValue; //Hodnota drzenych karet
	card cards[MAX_CARDS_HAND]; //Pole drzenych karet
	size_t nonsense; //Pocet nesmyslnych zprav
	char nickname[MAX_NICKNAME_LENGTH]; //Prezdivka klienta
	
	//Buffer zprav k odesilani a prijimani
	message_queue_t sendBuffer; 
	message_t sendingBuffer;
	size_t currentSendingByte;
	message_t receivingBuffer;
	size_t currentReceivingByte;
} client;


//Struktura odpojeneho klienta
typedef struct {
	
	int gameId; //Cislo hry klienta
	int credit; //Aktualni kredit
	int currBet; //Aktualni nabidka
	int cardValue; //Hodnota drzenych karet
	card cards[MAX_CARDS_HAND]; //Pole drzenych karet
	enum state state; //Aktualni stav
	size_t nonsense;  //Pocet nesmyslnych zprav
	char nickname[MAX_NICKNAME_LENGTH]; //Prezdivka klienta
} disconnectedClient;

//Funkce ziskani retezce adresy klienta 'peer'
char *peerGetAddressStr(client *peer);

//Funkce priani zpravy 'message' k odeslani klientem 'peer'
int peerAddToSend(client *peer, message_t *message);

//Funkce vytvoreni klienta 'peer'
int createClient(client *peer);


#endif
