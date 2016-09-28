#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

#include "types.h"
#include "comm.h"
#include "states.h"

void connect();
void clt_sigRutine(int);
void run_session();
void user_login();
void play_game();
void testing();
void client_close();

Data * newData(Opcode);


char user_input[BUFFERSIZE];

int session_state;

bool connected;

Connection * connection;
Data * data_from_server;
Data * data_to_send;

void main(int argc, char *argv[]) {

    connected=false;

    signal(SIGINT, clt_sigRutine);

    printf("[client] tying to connect to server on address %s\n", CONNECTION_ADDRESS);

    connection = comm_connect(CONNECTION_ADDRESS);

    if (connection==NULL)
        exit(1);

    connected=true;

    printf("[client] connection established\n");

    run_session();

    printf("[client] session done\n");

}

void run_session() {

    session_state = TESTING;

    while(1) {

        if(session_state == END_SESSION) {

            sendData(connection, newData(END_OF_CONNECTION));

            client_close();

            return ;

        } else if(session_state == USER_LOGIN) {

            user_login();

        } else if(session_state == PLAY_GAME) {

            play_game();

        } else if(session_state == TESTING) {

            testing();

        }

    }

}

void user_login() {



}

void play_game() {



}

void testing() {

	printf("[client] enter message to server (\"quit\" to quit): ");

	fgets(user_input, BUFFERSIZE, stdin);

        if(strcmp(user_input, "quit\n") == 0) {

            session_state = END_SESSION;

            return ;

        } else {

            data_to_send = newData(TEST_MESSAGE_STRING);

            strcpy(data_to_send->avmdata.message, user_input);

            sendData(connection, data_to_send);
    
        }
}

void client_close() {
    if (connected) {
        comm_disconnect(connection);
        free(connection);
    }
}

void clt_sigRutine(int sig) {
    
    //TODO: no deberia ser el mismo opcode que END_OF_CONNECTION , sino uno distinto. Para el handler de ese opcode usar kill en vez de exit¿?
    sendData(connection, newData(CONNECTION_INTERRUMPED));
    client_close();
    
    printf("\n");
    printf("Client proccess with pid: %d terminated\n", getpid());
    exit(1); 
}

