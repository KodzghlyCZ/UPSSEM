#ifndef COMMANDS_H_
#define COMMANDS_H_

#include "client.h"
#include "card.h"

//Definice prazdneho prikazu nebo segmentu zpravy
#define EMPTY_PART ""

//Vypis jednotlivych prikazu, viz dokumentace
#define COMMAND_CONNECT "CONN"
#define COMMAND_SEND_NICKNAME "SENDNCKN"
#define COMMAND_SET_LOBBY "SETLBY"
#define COMMAND_LOBBY_BUTTON "LBYBTN"
#define COMMAND_SET_GAME "SETGAME"
#define COMMAND_ADD_ROOM "LADDROOM"
#define COMMAND_JOIN_ROOM "JOINROOM"
#define COMMAND_GAME_READY "GAMERDY"
#define COMMAND_PLAYER_NOT_READY "PLRNRDY"
#define COMMAND_PLAYER_READY "PLRRDY"
#define COMMAND_PLACE_BET "PLACEBET"
#define COMMAND_UPDATE_CREDIT "UPDCRDT"
#define COMMAND_UPDATE_BET "UPDBET"
#define COMMAND_DEAL_CARD "DEALCARD"
#define COMMAND_CLEAR_CARDS "CLRCARDS"
#define COMMAND_PAUSE "PAUSE"
#define COMMAND_UNPAUSE "UNPAUSE"
#define COMMAND_HIT "HIT"
#define COMMAND_STAY "STAY"
#define COMMAND_BUST "BUST"
#define COMMAND_OK "OK"
#define COMMAND_SHOW_READY_BUTTON "SHBTNRDY"
#define COMMAND_HIGHER_THAN_DEALER "HGRDLR"
#define COMMAND_SHOW_BET_UI "SHBETUI"
#define COMMAND_LEAVE "LEAVE"
#define COMMAND_WIN "WIN"
#define COMMAND_LOSE "LOSE"
#define COMMAND_DEALER_BUSTS "DLRBUSTS"
#define COMMAND_YOU_TURN "YOUTURN"
#define COMMAND_PLAYER_TURNS "PLRTURNS"
#define COMMAND_DISCONNECTED "PLRDSCN"
#define COMMAND_RECONNECTED "PLRRCN"

//Funkce zaslani prikazu presunu do Lobby klientovi 'client'
uint8_t sendLobbyCommand(client *client);

//Funkce zaslani prikazu presunu do hry kleitnovi 'client'
uint8_t sendGameCommand(client *client);

//Funkce pridavajici klienta 'client' co mistnosti cislo 'gameId'
uint8_t addLobbyRoom(int gameId, client *client);

//Funnkce pro zaslani o stavu pripraveneho hrace 'nickname' osatanim hracum
uint8_t sendPlayerReady(client *client, char *nickname);

//Funnkce pro zaslani o stavu nepripraveneho hrace 'nickname' osatanim hracum
uint8_t sendPlayerNotReady(client *client, char *nickname);

//Funkce pro aktualizaci hodnoty kreditu hrace 'client'
uint8_t updatePlayerCredit(client *client);

//Funkce aktualzace nabidky hrace 'client'
uint8_t updatePlayerBet(client *client);

//Funkce zaslani typu karet hraci 'client'
uint8_t sendDealCard(client *client, card *card);

//Funkce aktualizace typu karet hraci 'client'
uint8_t updatePlayerCards(client *client);

//Funkce zaslani zpravy pro odstraneni zaslanych karet v rozhrani klienta 'client'
uint8_t sendClearCards(client *client);

//Funkce pro pozastaveni hry hrace 'klient'
uint8_t sendPause(client *client);

//Funkce pro spusteni hry hrace 'klient'
uint8_t sendUnpause(client *client);

//Funkce pro zaslani stavu 'bust' hraci 'klient'
uint8_t sendBust(client *client);

//Funkce pro zobrazeni prvku pro zadani sazky hrace 'client'
uint8_t sendShowBetUI(client *client);

//Funkce pro odeslani odezvy klientovi 'client'
uint8_t sendOk(client *client);

//Funkce zaslani tlacitka lobby klientovi 'client'
uint8_t sendLobbyButton(client *client);

//Funkce zaslani zobrazeni tlacitka stavu 'pripraven' hraci 'client'
uint8_t sendShowReadyButton(client *client);

//Funkce zaslani stavu znovupripojeni hrace s prezdivkou 'nickname' hraci 'client'
uint8_t sendPlayerReconnected(client *client, char *nickname);

//Funkce zaslani stavu odpojeni hrace s prezdivkou 'nickname' hraci 'client'
uint8_t sendPlayerDisconnected(client *client, char *nickname);

//Funkce zaslani stavu tahu hrace s prezdivkou 'nickname' hraci 'client'
uint8_t sendPlayerTurns(client *client, char *nickname);

//Funkce zaslani stavu tahu hraci 'client'
uint8_t sendYouTurn(client *client);

//Funkce zaslani stavu prekroceni dealera hraci 'klient'
uint8_t sendDealerBusts(client *client);

//Funkce zaslani stavu vyhry hraci 'klient'
uint8_t sendWin(client *client);

//Funkce zaslani stavu prohry hraci 'klient'
uint8_t sendLose(client *client);

#endif
