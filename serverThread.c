#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <pthread.h>

#include "types.h"
#include "comm.h"
#include "databasecomm.h"
#include "daemon.h"
#include "constants.h"

void srv_sigRutine(int);
void initDB_calls();
void server_close();
void server_process_data();
void communicate_with_database();
void * newSession(void *);

Data * receiveData(Connection *);
Data * db_receiveData(DBConnection *);

char * getaddress();

int session_ended = 0;

Data * data_from_client;
Data * data_to_client;

char character[100];
int totalExp;

int session;
int current_session;
pthread_t thread_id[MAX_THREADS];

Listener * listener;
Connection * connection;
DBConnection * db_connection;



int main(int argc, char *argv[]) {

	session=0;

	printf("[server] initializing\n");

    char * address = getaddress("SV");

    signal(SIGINT, srv_sigRutine);

    if (initLogin(false) == -1) {

    	printf("Couldn´t create message queue for logging daemon\n");

    }

    sndMessage("Server initialized", 1);

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

		pthread_create(&thread_id[session++], NULL ,&newSession, connection );	
	}

}

void * newSession(void * args) {


	Connection * current_connection = (Connection *) args;

	printf("[session %d] new client session started\n", session);
	sndMessage("new client session started", INFO_TYPE);

	int aux = session;

	while (1) {

		while (1) {
			char buffer[40];
			sprintf(buffer, "GG %d", aux);
			sndMessage(buffer, 1);
		}

		data_from_client = receiveData(current_connection);

		current_session = aux;

		server_process_data();

		sendData(current_connection, data_to_client);

		if(session_ended) {

			comm_disconnect(current_connection);

			return ;

		}

	}

}

void server_process_data() {

	int server_session = current_session;

	if(data_from_client->opcode == SELECT_CHARACTER) {

		communicate_with_database();

	} else if(data_from_client->opcode == CREATE_CHARACTER) {

		printf("[session %d] character created\n", server_session);

		sndMessage("Character created", INFO_TYPE);

		communicate_with_database();

	} else if(data_from_client->opcode == DELETE_CHARACTER) {

		printf("[session %d] character deleted\n", server_session);

		sndMessage("Character deleted", INFO_TYPE);

		communicate_with_database();

	} else if(data_from_client->opcode == EXP_UP) {

		communicate_with_database();

	} else if(data_from_client->opcode == EXIT) {

		printf("[session %d] session ended\n", server_session);
		sndMessage("server session ended", INFO_TYPE);

		session_ended = 1;

	} else if(data_from_client->opcode == EXIT_AND_LOGOUT) {

		printf("[session %d] session ended\n", server_session);
		sndMessage("server session ended", INFO_TYPE);

		communicate_with_database();

		session_ended = 1;

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

void srv_sigRutine(int sig) {

	sndMessage("Server logged out by kill()", WARNING_TYPE);
    
    exit(1);

}
