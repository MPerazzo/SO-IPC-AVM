#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "data.h"

void initChannel(int);
void initDB();

Data data;
Datagram datagram;
Connection sender;

int main(int argc, char *argv[])
{

    initChannel(1);
    initDB();
    
    while(1) {

    	//receiveData from Client
        receiveData(&sender, &datagram);
        //printf("Arguments are %s\n", datagram.arguments);
        
        printf("SV - receiveData passed\n");

        //Transform byte-flow to data
        unMarshall(&datagram, &data);
        printf("SV - Data opcode value is %d\n", data.opcode);
        printf("SV - Data pid value is %d\n", data.client_pid);
        printf("SV - Data number value is %d\n", data.avmdata.number);


        if (data.avmdata.number==3)
            printf("Unmarshall worked as expected\n");
        

        //Simulating modification of data from server
        data.avmdata.number=5000;
        strcpy(data.avmdata.message, "KBOY");

        free(datagram.arguments);

        //Transform data to byte-flow
    	Marshall(&datagram, &data);

        sendData(&sender, &datagram);
        
    }
}

void initDB() {

}