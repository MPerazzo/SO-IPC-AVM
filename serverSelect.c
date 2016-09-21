#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "types.h"
#include "comms.h"

void initDB_calls();
void Attend(Connection *, Data *);

Data data;
Datagram datagram;
Connection sender;

fd_set active_fd_set, read_fd_set;

int main(int argc, char *argv[])
{
    Connection * serverConnection;

    serverConnection = initChannel(1);
    
    if ( initLogin(false) == -1 ) {
    	printf("Couldn´t create message queue for server\n");
    	exit(1);
    }
    
    sndMessage("Hi Mate", 1);
    initDB_calls();

    FD_ZERO(&active_fd_set);
    FD_SET(serverConnection->id, &active_fd_set);

    while(1) {

        read_fd_set = active_fd_set;

        if (select (FD_SETSIZE, &read_fd_set, NULL, NULL, NULL) < 0) {
            perror("select failed\n");
            exit(1);
        }

        int i;
        for (i = 0; i < FD_SETSIZE; i++) {
            
            if (FD_ISSET(i, &read_fd_set)) {
        
                if (i == serverConnection->id) {
                    /* Connection request on server. */
                    acceptConnection(&sender);
                    FD_SET(sender.id, &active_fd_set);
                }
            
                else {
                    /* Data arriving on an already-connected socket. */
                    
                    sender.id = i; // or sender.id=listener.id or get_requestid();
                    receiveData(&sender, &datagram);
                    printf("SV - receiveData passed\n");
                    
                    int fpid;

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

                            sender.id = i;
                            sendData(&sender, &datagram);
                
                            //Kill child to avoid it from entering the loop and create infinite childs.
                            exit(0);
                        default:
                            FD_CLR (i, &active_fd_set);
           
                    }
                }
            }
        }
    }           
}

void initDB_calls() {

}

void Attend(Connection * sender, Data * data) {
    //Simulating server behaviour on database
    data->avmdata.number=5000;
    strcpy(data->avmdata.message, "KBOY");
}
