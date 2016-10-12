#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

#include "types.h"
#include "comm.h"
#include "databasecomm.h"
#include "daemon.h"
#include "constants.h"

void srv_sigRutine(int);
void newSession(Connection *);
void server_close();
void server_process_data();

char * getaddress();
Data * receiveData(Connection *);

Data * loginC(Data *);
Data * createaccountC(Data *);
Data * createcharC(Data *);
Data * deletecharC(Data *);
Data * showcharC(Data *);
Data * selectcharC(Data *);
Data * expUpC(Data *);
Data * logoutC(Data *);

bool session_ended = false;

Data * data_from_client;
Data * data_to_client;

Listener * listener;
Connection * connection;

int main(int argc, char *argv[]) {

	printf("[server] initializing\n");

    char * address = getaddress("SV");

    signal(SIGINT, srv_sigRutine);

    if (initLogin(false) == -1) {

    	printf("Couldn´t connect to logging daemon\n");

    }

    sndMessage("initializing server", INFO_TYPE);

	listener = comm_listen(address);

	if (listener == NULL) {

		sndMessage("couldn´t create listener", ERROR_TYPE);

		exit(1);

	}

	printf("[server] awaiting connection requests\n");
	sndMessage("ready to receive connection requests", INFO_TYPE);

	while(1) {

		connection = comm_accept(listener);

		if (connection != NULL) {

			int newpid = fork();

			if(newpid == 0) {

				newSession(connection);

				return 0;
			}

		}
		else
			sndMessage("couldn't accept connection from client", ERROR_TYPE);
	}

}

void newSession(Connection * connection) {

	printf("[session %d] new client session started\n", getpid());
	sndMessage("new client session started", INFO_TYPE);

	while (1) {

		data_from_client = receiveData(connection);

		server_process_data();

		if(session_ended) {

			server_close();

			return ;

		}
		sendData(connection, data_to_client);
	}

}

void server_process_data() {

	switch(data_from_client->opcode) {

		case LOGIN:

			data_to_client = loginC(data_from_client);
			break;

		case CREATE_ACCOUNT:

			data_to_client = createaccountC(data_from_client);
			break;

		case CREATE_CHARACTER:

			data_to_client = createcharC(data_from_client);
			break;
		
		case DELETE_CHARACTER:

			data_to_client = deletecharC(data_from_client);
			break;

		case SHOW_CHARACTER:

			data_to_client = showcharC(data_from_client);
			break;

		case SELECT_CHARACTER:

			data_to_client = selectcharC(data_from_client);
			break;

		case EXP_UP:

			data_to_client = expUpC(data_from_client);
			break;
		
		case LOGOUT:

			data_to_client = logoutC(data_from_client);
			break;

		case EXIT_AND_LOGOUT:

			printf("[session %d] session ended\n", getpid());
			sndMessage("server session ended", INFO_TYPE);

			logoutC(data_from_client);

			session_ended = true;

			break;

		case EXIT:

			printf("[session %d] session ended\n", getpid());
			sndMessage("server session ended", INFO_TYPE);

			session_ended = true;

			break;

		case CONNECTION_INTERRUMPED:

			printf("[session %d] session ended, CONNECTION_INTERRUMPED opcode received\n", getpid());
			sndMessage("Server is logged out by kill on client", WARNING_TYPE);

			session_ended = true;

			break;

		default:
			printf("Opcode not supported\n");
	}
}

void server_close() {
	comm_disconnect(connection);
	free(connection);
    close_daemoncomms();
}

void srv_sigRutine(int sig) {

    sndMessage("Server logged out by kill()", WARNING_TYPE);

    close_daemoncomms();

    printf("\n");
    printf("Server proccess with pid: %d terminated\n", getpid());

    exit(1);
}
