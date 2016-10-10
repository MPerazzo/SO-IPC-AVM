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
#include "semaphores.h"

void srv_sigRutine(int);
void initDB_calls();
void newSession(Connection *);
void server_close();
void server_process_data();

char * getaddress();
Data * receiveData(Connection *);
Data * db_receiveData(DBConnection *);

void loginC();
void createaccountC();
void createcharC();
void deletecharC();
void showcharC();
void selectcharC();
void expUpC();
void logoutC();
void exitC();

int session_ended = false;

Data * data_from_client;
Data * data_to_client;

Listener * listener;
Connection * connection;
DBConnection * db_connection;

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

		if (connection == NULL) {

			sndMessage("couldn't accept connection from client", ERROR_TYPE);

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

	printf("[session %d] new client session started\n", getpid());
	sndMessage("new client session started", INFO_TYPE);

	while (1) {

		data_from_client = receiveData(connection);

		server_process_data();

		sendData(connection, data_to_client);

		if(session_ended) {

			server_close();

			return ;

		}

	}

}

void server_process_data() {

	switch(data_from_client->opcode) {

		case LOGIN:

			loginC();
			break;

		case CREATE_ACCOUNT:

			createaccountC();
			break;

		case CREATE_CHARACTER:

			createcharC();
			break;
		
		case DELETE_CHARACTER:

			deletecharC();
			break;

		case SHOW_CHARACTER:

			showcharC();
			break;

		case SELECT_CHARACTER:

			selectcharC();
			break;

		case EXP_UP:

			expUpC();
			break;
		
		case LOGOUT:

			logoutC();
			break;

		case EXIT:

			printf("[session %d] session ended\n", getpid());
			sndMessage("server session ended", INFO_TYPE);

		    exitC();

			session_ended = true;

			break;

		case EXIT_AND_LOGOUT:

			printf("[session %d] session ended\n", getpid());
			sndMessage("server session ended", INFO_TYPE);

			logoutC();

		    exitC();

			session_ended = true;

			break;

		case CONNECTION_INTERRUMPED:

			printf("[session %d] session ended, CONNECTION_INTERRUMPED opcode received\n", getpid());
			sndMessage("Server is logged out by kill on client", WARNING_TYPE);

		    server_close();

			exit(1);

		default:
			printf("Opcode not supported\n");
	}
}

void communicate_with_database() {

	db_connection = db_comm_connect(getaddress("DBSV"));

	if(db_connection == NULL) {

		sndMessage("couldn't connect to database", ERROR_TYPE);

		exit(1);
	}

	db_sendData(db_connection, data_from_client);

	data_to_client = db_receiveData(db_connection);

	db_comm_disconnect(db_connection);
}

void server_close() {
	comm_disconnect(connection);
	free(connection);
    close_daemon();
}

void srv_sigRutine(int sig) {

    sndMessage("Server logged out by kill()", WARNING_TYPE);

    close_daemon();

    printf("\n");
    printf("Server proccess with pid: %d terminated\n", getpid());

    exit(1);
}
