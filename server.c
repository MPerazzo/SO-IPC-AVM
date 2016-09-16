#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "data.h"

void initChannel(int);
void initDB();
void Attend(Connection *, Data *);

Data data;
Datagram datagram;
Connection sender;

int main(int argc, char *argv[])
{

    initChannel(1);
    initDB();
    
    int fpid;

    while(1) {

    	//receiveData from Client
        receiveData(&sender, &datagram);
        
        printf("SV - receiveData passed\n");

        switch (fpid= fork()) {
		
		case -1:
			perror("Couldn´t fork server\n");
			exit(1);
		case 0:
			//Transform byte-flow to data
			unMarshall(&datagram, &data);

			if (data.avmdata.number==3)
        		printf("SV - Unmarshall worked as expected\n");

			printf("SV - Data opcode value is %d\n", data.opcode);
        		printf("SV - Data pid value is %d\n", data.client_pid);
        		printf("SV - Data number value is %d\n", data.avmdata.number);			
			
			Attend(&sender, &data);
			
			free(datagram.arguments);

        		//Transform data to byte-flow
    			Marshall(&datagram, &data);

			sendData(&sender, &datagram);
			
			//Kill child to avoid it from entering the loop and create infinite childs.
			exit(0);
       
		}        
    }
}

void initDB() {

}

void Attend(Connection * sender, Data * data) {
    //Simulating server behaviour on database
    data->avmdata.number=5000;
    strcpy(data->avmdata.message, "KBOY");
}
