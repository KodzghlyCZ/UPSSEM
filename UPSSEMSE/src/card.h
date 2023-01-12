#ifndef CARD_H_
#define CARD_H_

//Pocet karet v jednom balicku
#define DECK_CARDS 52
//Delka nazvu jmena karty
#define CARD_NAME_LENGTH 2

//Struktura karty

typedef struct{
	char name[CARD_NAME_LENGTH+1];
	uint8_t value;
}card;

#endif
