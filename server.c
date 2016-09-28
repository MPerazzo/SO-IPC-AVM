#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

#include "types.h"
#include "comm.h"
#include "daemon.h"
#include "states.h"

void srv_sigRutine(int);
void initDB_calls();
void newSession(Connection *);
void server_close();

char * getadress();
Data * receiveData(Connection *);

Listener * listener;
Connection * connection;

int main(int argc, char *argv[])
{
	signal(SIGINT, srv_sigRutine);

	if ( initLogin(false) == -1 ) {
    	printf("Couldn´t create message queue for server\n");
    	exit(1);
    }
    
    sndMessage("Server is logged in", INFO_TYPE); // sends message to daemonServer
    
    initDB_calls();

    char * address = getadress();

	listener = comm_listen(address);

	if (listener==NULL) {
		sndMessage("Couldn´t create listener", ERROR_TYPE);
		exit(1);
	}

	printf("[server] awaiting connection requests\n");

	while(1) {

		connection = comm_accept(listener);

		if (connection==NULL) {
			sndMessage("Couldn´t create connection from listener", ERROR_TYPE);
			exit(1);
		}

		int newpid = fork();

		if(newpid == 0) {

			newSession(connection);

			return 0;
		
		}

	}

}

void newSession(Connection * connection) {

	Data * data_from_client;
	Data * data_to_send;

	printf("[session %d] new client session started\n", getpid());

	while (1) {

		data_from_client = receiveData(connection);

		if (data_from_client == NULL) {
			exit(1);
		}

		if(data_from_client->opcode == END_OF_CONNECTION) {

			printf("[session %d] session ended, END_OF_CONNECTION opcode received\n", getpid());

			sndMessage("Server is logged out with no errors", INFO_TYPE);

			server_close();

			return;

		}  else if(data_from_client->opcode == CONNECTION_INTERRUMPED) {

			printf("[session %d] session ended, CONNECTION_INTERRUMPED opcode received\n", getpid());

			sndMessage("Server is logged out by kill on client", WARNING_TYPE);

			server_close();

			return;

		} else if(data_from_client->opcode == TEST_MESSAGE_STRING) {

			printf("[session %d] received message: %s", getpid(), data_from_client->avmdata.message);

		} else {

			printf("[session %d] error: unknown operation requested\n", getpid());

		}
	}
}

void initDB_calls() {

}

void srv_sigRutine(int sig) {

    printf("\n");
    printf("Server proccess with pid: %d terminated\n", getpid());

    sndMessage("Server logged out by kill()", WARNING_TYPE);
    
    exit(1);
}

void server_close() {
	comm_disconnect(connection);
	free(listener);
	free(connection);
}