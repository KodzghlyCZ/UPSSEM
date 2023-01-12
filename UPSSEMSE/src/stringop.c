#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "message.h"

#include "stringop.h"

int randString(char *str, int size)
{
	const char charset[] =
	    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	if (size) {
		--size;
		for (int n = 0; n < size; n++) {
			int key = rand() % (int)(sizeof charset - 1);
			str[n] = charset[key];
		}
	}
	return 0;

}

void replaceChar(char *str, char old_char, char new_char)
{
	for (size_t i = 0; i < strlen(str); i++) {
		if (str[i] == old_char) {
			str[i] = new_char;
		}
	}
}

uint8_t concatMessage(char *messageType, char *messageContent,
		      char *messageTail, char *targetString)
{
	char messageTypeFormatted[MESSAGE_PART_SIZE + 1];
	sprintf(messageTypeFormatted, "%-*.*s", MESSAGE_PART_SIZE,
		MESSAGE_PART_SIZE, messageType);
	char messageContentFormatted[MESSAGE_PART_SIZE + 1];
	sprintf(messageContentFormatted, "%-*.*s", MESSAGE_PART_SIZE,
		MESSAGE_PART_SIZE, messageContent);
	char messageTailFormatted[MESSAGE_PART_SIZE + 1];
	sprintf(messageTailFormatted, "%-*.*s", MESSAGE_PART_SIZE,
		MESSAGE_PART_SIZE, messageTail);

	strcpy(targetString, messageTypeFormatted);
	strcat(targetString, messageContentFormatted);
	strcat(targetString, messageTailFormatted);

	return 0;
}
