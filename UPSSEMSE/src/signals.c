#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "server.h"

#include "signals.h"

void handleSignalAction(int sigNumber)
{
	if (sigNumber == SIGINT) {
		printf("\nSIGINT was catched!\n");
		shutdownProperly(EXIT_SUCCESS);
	} else if (sigNumber == SIGPIPE) {
		printf("\nSIGPIPE was catched!\n");
		shutdownProperly(EXIT_SUCCESS);
	}
}

int setupSignals()
{
	struct sigaction sa;
	sa.sa_handler = handleSignalAction;
	if (sigaction(SIGINT, &sa, 0) != 0) {
		perror("sigaction()");
		return -1;
	}
	if (sigaction(SIGPIPE, &sa, 0) != 0) {
		perror("sigaction()");
		return -1;
	}
	return 0;
}
