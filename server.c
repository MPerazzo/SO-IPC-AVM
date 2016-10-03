#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

#include "types.h"
#include "comm.h"
#include "daemon.h"
#include "constants.h"

void srv_sigRutine(int);
void initDB_calls();
void newSession(Connection *);
void server_close();

char * getaddress();
Data * receiveData(Connection *);
Data * newData(Opcode opcode);

bool connected_toclt;

Listener * listener;
Connection * connection;

int main(int argc, char *argv[])
{
	connected_toclt = false;

	signal(SIGINT, srv_sigRutine);

	if ( initLogin(false) == -1 ) {
    	printf("Couldn´t create message queue for server\n");
    	exit(1);
    }
    
    sndMessage("Server is logged in", INFO_TYPE); // sends message to daemonServer
    
    initDB_calls();

    char * address = getaddress();

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

		connected_toclt = true;

		int newpid = fork();

		if(newpid == 0) {

			sndMessage("New connection", INFO_TYPE);


			newSession(connection);

			return 0;
		
		}

	}

}

void newSession(Connection * connection) {

	Data * data_from_client;
	Data * data_to_send;
	Character characters[MAX_CHARACTERS];
	User user;
	printf("[session %d] new client session started\n", getpid());

	while (1) {

		data_from_client = receiveData(connection);

		if (data_from_client == NULL) {
			exit(1);
		}

		if(data_from_client->opcode == LOGIN) {

			sndMessage("New login", INFO_TYPE);


			if(strcmp(user.account, data_from_client->avmdata.user.account) == 0 && strcmp(user.password, data_from_client->avmdata.user.password) == 0) {
				data_to_send = newData(NO_ERR);
			} else {
				data_to_send = newData(ERR_PARAMETER);
			}
			sendData(connection, data_to_send);

		} else if(data_from_client->opcode == CREATE_ACCOUNT) {

			sndMessage("Account created", INFO_TYPE);

			strcpy(user.account, data_from_client->avmdata.user.account);
			strcpy(user.password, data_from_client->avmdata.user.password);

			data_to_send = newData(NO_ERR);

        	sendData(connection, data_to_send);

		} else if(data_from_client->opcode == SELECT_CHARACTER) {
			int i, flag = 0;
			for(i = 0 ; i < MAX_CHARACTERS ; i++) {
				if(strcmp(characters[i].name, data_from_client->avmdata.charSelected.name) == 0) {
					flag = 1;
					data_to_send = newData(NO_ERR);
					strcpy(data_to_send->avmdata.charSelected.name, characters[i].name);
					data_to_send->avmdata.charSelected.lvl = characters[i].lvl;
					data_to_send->avmdata.charSelected.totalExp = characters[i].totalExp;
					data_to_send->avmdata.charSelected.currentExp = characters[i].currentExp;
				}
			}
			if(!flag) {
				data_to_send = newData(ERR_PARAMETER);
			}
			sendData(connection, data_to_send);

		} else if(data_from_client->opcode == CREATE_CHARACTER) {

			strcpy(characters[1].name, data_from_client->avmdata.charSelected.name);
			characters[1].lvl = 1;
			characters[1].totalExp = 10;
			data_to_send = newData(NO_ERR);

        	sendData(connection, data_to_send);

		} else if(data_from_client->opcode == SHOW_CHARACTER) {

		} else if(data_from_client->opcode == SAVE_STATS) {

			characters[1].lvl = data_from_client->avmdata.charSelected.lvl;
			characters[1].totalExp = data_from_client->avmdata.charSelected.totalExp;
			characters[1].currentExp = data_from_client->avmdata.charSelected.currentExp;
			printf("CHARACTER: name:%s lvl:%d exp: %d/%d\n", characters[1].name, characters[1].lvl,characters[1].currentExp, characters[1].totalExp);

		} else if(data_from_client->opcode == END_OF_CONNECTION) {

			printf("[session %d] session ended, END_OF_CONNECTION opcode received\n", getpid());

			sndMessage("Server is logged out with no errors", INFO_TYPE);

			server_close();

			return;

		}  else if(data_from_client->opcode == CONNECTION_INTERRUMPED) {

			printf("[session %d] session ended, CONNECTION_INTERRUMPED opcode received\n", getpid());

			sndMessage("Server is logged out by kill on client", WARNING_TYPE);

			server_close();

			return;

		} else {

			printf("[session %d] error: unknown operation requested\n", getpid());

		}
	}
}

void initDB_calls() {

}

void server_close() {
	if (connected_toclt == true) {
		comm_disconnect(connection);
		free(connection);
	}
}

void srv_sigRutine(int sig) {

    printf("\n");
    printf("Server proccess with pid: %d terminated\n", getpid());

    sndMessage("Server logged out by kill()", WARNING_TYPE);

    free(listener);

    server_close();
    
    exit(1);
}