
#include "data.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void initChannel(int);
void Recieve();
void communicate(Connection *, Datagram *);

Data data;
Datagram datagram;
Connection sender;

int main(int argc, char *argv[])
{

    initChannel(0);

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
 
}

void communicate(Connection * sender, Datagram * datagr) {
	if (sendData(sender, datagr) != -1)
		receiveData(sender, datagr);
}