#ifndef SIGNALS_H_
#define SIGNALS_H_

//Zpracovani signalu cisla 'sigNumber'
void handleSignalAction(int sigNumber);

//Funkce nastaveni a zavedeni vsech potrebnych signalu
int setupSignals();

#endif
