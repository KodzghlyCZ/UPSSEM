#ifndef STRINGOP_H_
#define STRINGOP_H_

//Funkce generace nahodneho retezce o delce 'size' a jeho ulozeni do mista 'str'
int randString(char *str,int size);

//Funkce pro nahrazeni vyskytu znaku 'old_char' za 'new_char' v retezci 'str'
void replaceChar(char* str, char old_char, char new_char);

uint8_t concatMessage(char *messageType, char *messageContent, char *messageTail, char *targetString);

#endif
