#ifndef GAME_H_
#define GAME_H_

#include <stddef.h>

#include "client.h"
#include "card.h"

//Pocet paralelnich her
#define MAX_GAMES 4
//Pocet kol hry
#define GAME_ROUNDS 2
//Maximalni pocet hracu ve hre
#define MAX_PLAYERS_IN_GAME 3

//Vychozi hodnota kreditu hrace 
#define DEFAULT_PLAYER_CREDIT 200

//Struktura uchovavajici informace o jedne instanci hry
typedef struct{
	int dealerCardsValue; 	//Hodnota karet dealera
	uint8_t isPaused;		//Je pozastavena
	size_t round;			//Cislo aktualniho kola
	size_t currPlayer;		//Index aktualniho hrace
	card *cards[DECK_CARDS];//Seznam karet kola hry
	client *players[MAX_PLAYERS_IN_GAME]; 	//Seznam hracu ve hre
	disconnectedClient *disconnectedPlayers[MAX_PLAYERS_IN_GAME]; //Seznam odpojenych hracu ve hre
} game;

//Funkce pro zjisteni bezici hry s cislem 'gameId'
uint8_t isGameRunning(int gameId);

//Funkce pro inicializaci struktur her
uint8_t initGames();

//Funkce pro overeni dostupneho mista ve hre s cislem 'gameId'
uint8_t hasSpace(int gameId);

//Funkce pro pruchod dostupnymi hrami
uint8_t iterateAvailableGames(uint8_t (*func)(int, client *), client *client);

//Funkce pridani hrace 'client' do hry s cislem 'gameId'
uint8_t addPlayer(int gameId, client *client);

//Funkce overeni stavu pripravenych hracu hry s cislem 'client'
uint8_t checkPlayersReady(int gameId);

//Funkce spusteni hry
uint8_t startGame(int gameId);

//Funkce zaslani zpravy stavu pripraveneho hrace 'client'
uint8_t broadcastReady(client *client);

//Funkce zaslani zpravy stavu nepripraveneho hrace 'client'
uint8_t broadcastNotReady(client *client);

//Funkce ulozeni udaju odpojeneho hrace 'client'
uint8_t suspendPlayer(client *client);

//Funkce zaslani stavu hrace 'client'
uint8_t sendPlayerStates(client *client);

//Funkce pro presunuti pripojeneho hrace 'client' do odpojenych hracu
uint8_t movePlayerToDisconnected(client *client);

//Funkce pro presunuti odpojeneho hrace 'client' do pripojenych hracu
uint8_t movePlayerToConnected(client *client);

//Funkce pro zahajeni noveho kola hry s cislem 'gameId'
uint8_t newRound(int gameId);

//Funkce kontroly polozene sazky vsech hracu hry s cislem 'gameId'
uint8_t checkBetsPlaced(int gameId);

//Funkce podani karty 'card' hraci 'client'
uint8_t dealCard(card *card, client *client);

//Funkce vypisu udaju o hre
uint8_t printGame(int gameId);

////Funkce kontroly vyhry na prvnim tahu hry s cislem 'gameId'
//uint8_t checkInitialWin(int gameId);

//Funkce overeni vsech hracu hry s cislem 'gameId' cekaji na nove kolo
uint8_t checkAllWaitingNewRound(int gameId);

//Funkce ověření pozastavení hry s číslem 'gameId'
uint8_t isPaused(int gameId);

//Funkce podani karty 'card' hraci 'client'
uint8_t dealGameCard(int gameId, client *client);

//Funkce akce "hit" hrace 'client'
uint8_t gameHit(client *client);

//Funkce akce "hit" hrace 'client'
uint8_t gameStay(client *client);

//Funkce pro posun gry s cislem 'gameId' do dalsiho kola
uint8_t nextTurn(int gameId);

//Funkce nastaveni stavu hrace 'client' na cekani na dalsi kolo nebo konec hry
uint8_t setPlayerWaitingRoundEnd(client *client);

//Funkce overeni vsech hracu hry s cislem 'gameId' dokoncili sve tahy
uint8_t checkRoundComplete(int gameId);

//Funkce pro ukonceni kola hry s cislem 'gameId'
uint8_t endRound(int gameId);

//Funkce pro ukonceni hry 'gameId'
uint8_t endGame(int gameId, int directEnd);

//Funkce pro podani karty dealerovi ve hre s cislem 'gameId'
uint8_t dealDealerCard(int gameId);

//Funkce pro pozastaveni hry hrace 'client'
uint8_t pauseGame(client *client);

//Funkce pro zotaveni hry hrace 'client'
uint8_t unpauseGame(client *client);

//Funkce vyslani zpravy nedostupnosti hrace 'client'
uint8_t broadcastPlayerDisconnected(client *client);

//Funkce vyslani zpravy znovupripojeni hrace 'client'
uint8_t broadcastPlayerReconnected(client *client);

//Funkce vyslani zpravy tahu hrace 'client'
uint8_t broadcastPlayerTurns(client *client);

//Funkce vyslani zpravy vyssi hodnoty karet nez dealer hrace 'client'
uint8_t sendHigherThanDealer(client *client);

//Funkce kontroly pritomnych hracu hry s cislem 'gameId'
uint8_t hasActivePlayers(int gameId);

#endif
