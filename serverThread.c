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
void * newSession(void * args);
Data * server_process_data(int current_session, bool * session_ended, Data * data_from_client);
void server_close();

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
void exitC();

int session;
pthread_t thread_id[MAX_THREADS];

Listener * listener;
Connection * connection;

int main(int argc, char *argv[]) {

	session = FIRST_SESSION -1;

	printf("[server] initializing\n");

    char * address = getaddress("SV");

    signal(SIGINT, srv_sigRutine);

    if (initLogin(false) < 0) {

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

			if (session == MAX_THREADS) {
				printf("Thread limit reached\n");
				sleep(FOREVER);
			}

			pthread_create(&thread_id[session++], NULL ,&newSession, connection );				
			
		}
		else
			sndMessage("couldn't accept connection from client", ERROR_TYPE);
	}

}

void * newSession(void * args) {

	Connection * current_connection = (Connection *) args;

	Data * data_from_client;
	Data * data_to_client;

	printf("[session %d] new client session started\n", session);
	sndMessage("new client session started", INFO_TYPE);

	int current_session = session;

	bool session_ended = false;

	while (1) {

		data_from_client = receiveData(current_connection);

		data_to_client = server_process_data(current_session, &session_ended, data_from_client);

		if(session_ended) {
			server_close(current_connection);
			return ;
		}

		sendData(current_connection, data_to_client);
	}

}

Data * server_process_data(int current_session, bool * session_ended, Data * data_from_client) {

	switch(data_from_client->opcode) {

		case LOGIN:

			return loginC(data_from_client);

		case CREATE_ACCOUNT:

			return createaccountC(data_from_client);

		case CREATE_CHARACTER:

			return createcharC(data_from_client);
		
		case DELETE_CHARACTER:

			return deletecharC(data_from_client);

		case SHOW_CHARACTER:

			return showcharC(data_from_client);

		case SELECT_CHARACTER:

			return selectcharC(data_from_client);

		case EXP_UP:

			return expUpC(data_from_client);
		
		case LOGOUT:

			return logoutC(data_from_client);

		case EXIT_AND_LOGOUT:

			printf("[session %d] session ended\n", current_session);
			sndMessage("server session ended", INFO_TYPE);

			logoutC(data_from_client);

			*session_ended = true;

			return NULL;

		case EXIT:

			printf("[session %d] session ended\n", current_session);
			sndMessage("server session ended", INFO_TYPE);

			*session_ended = true;

			return NULL;

		case CONNECTION_INTERRUMPED:

			printf("[session %d] session ended, CONNECTION_INTERRUMPED opcode received\n", current_session);
			sndMessage("Server is logged out by kill on client", WARNING_TYPE);

			*session_ended = true;

			return NULL;

		default:
			printf("Opcode not supported\n");
	}
}

void server_close(Connection * connection) {
	comm_disconnect(connection);
	free(connection);
}

void srv_sigRutine(int sig) {

    sndMessage("Server logged out by kill()", WARNING_TYPE);

    free(listener);

    close_daemoncomms();

    printf("\n");
    printf("Server proccess with pid: %d terminated\n", getpid());

    exit(1);
}
