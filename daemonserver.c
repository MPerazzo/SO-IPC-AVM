#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "types.h"
#include "daemon.h"

void daemon_sigRutine(int);

int main () {

    signal(SIGINT, daemon_sigRutine);

	if ( initLogin(true) == -1 ) {
	    	printf("Couldn´t init daemonServer\n");
	    	exit(1);
	}

	while (1) {
		
		rcvMessage(ALL_TYPES);
				
		printMessage();
	}
}

void daemon_sigRutine(int sig) {

    close_daemoncomms();

    printf("\n");
    printf("Daemon proccess with pid: %d terminated\n", getpid());

    exit(1);
}

