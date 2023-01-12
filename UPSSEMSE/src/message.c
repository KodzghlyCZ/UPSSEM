#include "message.h"

int prepareMessage(char *data, message_t * message)
{
	char buf[MESSAGE_SIZE];
	memset(buf, 0, MESSAGE_SIZE);
	strncpy(buf, data, strlen(data));
	strncpy(message->data, buf, MESSAGE_SIZE);
	return 0;
}
