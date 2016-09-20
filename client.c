
#include "types.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void communicate(Connection *, Datagram *);
Connection * reateConnection(int);
Connection * initChannel(int);

Data data;
Datagram datagram;
Connection sender;

int main(int argc, char *argv[])
{
    Connection * clientConnection;

    clientConnection=initChannel(0);

    sender.id=clientConnection->id;

    
    while(1) {
        //Simulating client request and Data
        void * newdata = calloc(sizeof(data), 1);
        memcpy(&data, newdata, sizeof(newdata));
        data.size = sizeof(data);
        data.opcode = 1;
        data.client_pid = 2;
        data.avmdata.number = 3;
       
        //Transform data to byte-flow
        Marshall(&datagram, &data);

        //Send and receive Datagram modified by server
        communicate(&sender, &datagram);

        //Transform byte-flow to data
        unMarshall(&datagram, &data);

        printf("Cli - Data opcode value is %d\n", data.opcode);
        printf("Cli - Data pid value is %d\n", data.client_pid);
        printf("Cli - Data number value is %d\n", data.avmdata.number);
        printf("Cli - Message from server is %s\n", data.avmdata.message);
        sleep(2);
    }

}

void communicate(Connection * sender, Datagram * datagr) {
	if (sendData(sender, datagr) != -1) 
		receiveData(sender, datagr);
    else {
        printf("Communication with server failed\n");
    }
}
