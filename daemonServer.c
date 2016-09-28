#include <stdio.h>
#include <stdlib.h>

#include "daemon.h"
#include "types.h"

int main () {

	if ( initLogin(true) == -1 ) {
	    	printf("Couldn´t create message queue for server\n");
	    	exit(1);
	}

	while (1) {
		
		rcvMessage(0);
		
		bool daemonSrv = true;
		
		printMessage(daemonSrv);
		
		_clearmsg();
	}
}
