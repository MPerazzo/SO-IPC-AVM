#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>

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
void communicate_with_database();

char * getaddress();

int session_ended = 0;

int semaphore_id;

Data * data_from_client;
Data * data_to_client;

char character[100];
int totalExp;

Listener * listener;
Connection * connection;
DBConnection * db_connection;

int main(int argc, char *argv[]) {

	printf("[server] initializing\n");
	sndMessage("initializing server", INFO_TYPE);

    char * address = getaddress("SV");

    signal(SIGINT, srv_sigRutine);

    if (initLogin(false) == -1) {

    	printf("Couldn´t create message queue for logging daemon\n");

    	exit(1);

    }

    semaphore_id = binary_semaphore_allocation (666, IPC_RMID);

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

			comm_disconnect(connection);

			return ;

		}

	}

}

void server_process_data() {

	if(data_from_client->opcode == SELECT_CHARACTER) {

		communicate_with_database();

	} else if(data_from_client->opcode == CREATE_CHARACTER) {

		communicate_with_database();

	} else if(data_from_client->opcode == DELETE_CHARACTER) {

		communicate_with_database();

	} else if(data_from_client->opcode == EXP_UP) {

		communicate_with_database();

	} else if(data_from_client->opcode == EXIT) {

		printf("[session %d] session ended\n", getpid());
		sndMessage("server session ended", INFO_TYPE);

		session_ended = 1;

	} else if(data_from_client->opcode == EXIT_AND_LOGOUT) {

		printf("[session %d] session ended\n", getpid());
		sndMessage("server session ended", INFO_TYPE);

		communicate_with_database();

		session_ended = 1;

	}

}

void communicate_with_database() {

	binary_semaphore_wait(semaphore_id);

	db_connection = db_comm_connect(getaddress("DBSV"));

	if(db_connection == NULL) {

		sndMessage("couldn't connect to database", ERROR_TYPE);

		exit(1);

	}

	db_sendData(db_connection, data_from_client);

	data_to_client = db_receiveData(db_connection);

	db_comm_disconnect(db_connection);

	binary_semaphore_post(semaphore_id);

}

void srv_sigRutine(int sig) {

    sndMessage("Server logged out by kill()", WARNING_TYPE);
    
    exit(1);

}
