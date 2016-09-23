#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

#include "types.h"
#include "comm.h"


int main(int argc, char *argv[]) {

    int counter = 0;
    Data * data;
    Data * serverResponse;
    Connection * connection;
    char * address = "/tmp/listener";

    data = malloc(sizeof(Data));
    data->size = sizeof(Data);
    data->client_pid = getpid();

    /*        */printf("[client] connecting to server on address %s\n", address);

    connection = comm_connect(address);

    /*        */printf("[client] connection established. sending data\n");

    while(counter < 200) {

	sleep(1);

        data->opcode = counter;

        sendData(connection, data);

        /*        */printf("[client] sent counter = %d\n", counter);

        serverResponse = receiveData(connection);

	counter = serverResponse->opcode;

        /*        */printf("[client] received counter = %d\n", counter);

	counter += 5;
    }

    /*        */printf("[client] done\n");

}
