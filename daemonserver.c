#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "types.h"

void daemon_sigRutine(int);

int main () {

    signal(SIGINT, daemon_sigRutine);

	if ( initLogin(true) == -1 ) {
	    	printf("Couldn´t init daemonServer\n");
	    	exit(1);
	}

	while (1) {
		
		rcvMessage(0);
				
		printMessage();
		
	}
}

void daemon_sigRutine(int sig) {

    close_daemon();

    printf("\n");
    printf("Daemon proccess with pid: %d terminated\n", getpid());

    exit(1);
}

