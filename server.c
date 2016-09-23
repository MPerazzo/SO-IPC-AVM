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
	char * address = "/tmp/listener";

	listener = comm_listen(address);

	/*        */printf("[server] awaiting connection requests\n");

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

	int counter = 0;	
	
	Data * data;

	/*        */printf("[session %d] new client session started\n", getpid());


	while(counter < 190) {

		data = receiveData(connection);

		counter = data->opcode;

		/*        */printf("[session %d] received counter = %d\n", getpid(), counter);

		counter += 5;

		data->opcode = counter;

		/*        */printf("[session %d] sending counter = %d\n", getpid(), counter);

		sendData(connection, data);

	}

	/*        */printf("[session %d] session done\n", getpid());

}
