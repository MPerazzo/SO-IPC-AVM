#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

#include "types.h"
#include "comm.h"

#define END_SESSION 0
#define USER_LOGIN 1
#define PLAY_GAME 2
#define TESTING 100

#define BUFFERSIZE 100

void run_session();
void user_login();
void play_game();
void testing();

char user_input[BUFFERSIZE];

int session_state;

Connection * connection;
Data * data_from_server;
Data * data_to_send;

void main(int argc, char *argv[]) {

    printf("[client] connecting to server on address %s\n", CONNECTION_ADDRESS);

    connection = comm_connect(CONNECTION_ADDRESS);

    printf("[client] connection established\n");

    run_session();

    printf("[client] session done\n");

}

void run_session() {

    session_state = TESTING;

    while(1) {

        if(session_state == END_SESSION) {

            sendData(connection, newData(END_OF_CONNECTION));

            comm_disconnect(connection);

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

    while(1) {

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
}

