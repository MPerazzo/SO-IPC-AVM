#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "types.h"
#include "comm.h"

void newSession(Connection *);

int main(int argc, char *argv[])
{
	int newpid;
	Listener * listener;
	Connection * connection;

	listener = comm_listen(CONNECTION_ADDRESS);

	printf("[server] awaiting connection requests\n");

	while(1) {

		connection = comm_accept(listener);

		newpid = fork();

		if(newpid == 0) {

			newSession(connection);

			break;
		
		} 

	}

}

void newSession(Connection * connection) {

	Data * data_from_client;
	Data * data_to_send;

	printf("[session %d] new client session started\n", getpid());

	while(1) {

		data_from_client = receiveData(connection);

		if(data_from_client->opcode == END_OF_CONNECTION) {

			comm_disconnect(connection);

			printf("[session %d] session ended remotely\n", getpid());

			break;

		} else if(data_from_client->opcode == TEST_MESSAGE_STRING) {

			printf("[session %d] received message: %s", getpid(), data_from_client->avmdata.message);

		} else {

			printf("[session %d] error: unknown operation requested\n", getpid());

		}

	}

}
