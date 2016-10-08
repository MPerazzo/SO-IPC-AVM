#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

#include "types.h"
#include "comm.h"
#include "constants.h"

Data * loginC(char *,char *, Connection *);
Data * createAccountC(char *, char *, Connection *);
Data * selectCharacterC(char *, char *, Connection *);
Data * createCharacterC(char *, char *, Connection *);
Data * deleteCharacterC(char *, char *, Connection *);
Data * showCharactersC(char *, Connection *);
void expUpC(Character, char *, Connection *);
void quitC(char *, Connection *, int);
void logoutC(char *, Connection *);
Data * communicate(Connection *, Data *);


Data * loginC(char * account,char * password, Connection * connection) {

    Data * data_from_server;
    Data * data_to_send = newData(LOGIN);

    strcpy(data_to_send->user.username, account);
    strcpy(data_to_send->user.password, password);

    data_from_server = communicate(connection, data_to_send);

    return data_from_server;
}

Data * createAccountC(char * account, char * password, Connection * connection) {

    Data * data_from_server;
    Data * data_to_send = newData(CREATE_ACCOUNT);

    strcpy(data_to_send->user.username, account);
    strcpy(data_to_send->user.password, password);

    data_from_server = communicate(connection, data_to_send);

    return data_from_server;
}

Data * selectCharacterC(char * name, char * account, Connection * connection) {

    Data * data_from_server;
    Data * data_to_send = newData(SELECT_CHARACTER);

    strcpy(data_to_send->character.name, name);
    strcpy(data_to_send->user.username, account);

    data_from_server = communicate(connection, data_to_send);

    return data_from_server;
}

Data * createCharacterC(char * name, char * account, Connection * connection) {

    Data * data_from_server;
    Data * data_to_send = newData(CREATE_CHARACTER);

    strcpy(data_to_send->character.name, name);
    strcpy(data_to_send->user.username, account);

    data_from_server = communicate(connection, data_to_send);

    return data_from_server;
}

Data * deleteCharacterC(char * name, char * account, Connection * connection) {

    Data * data_from_server;
    Data * data_to_send = newData(DELETE_CHARACTER);

    strcpy(data_to_send->character.name, name);
    strcpy(data_to_send->user.username, account);

    data_from_server = communicate(connection, data_to_send);

    return data_from_server;
}

Data * showCharactersC(char * account, Connection * connection) {

    Data * data_from_server;
    Data * data_to_send = newData(SHOW_CHARACTER);

    strcpy(data_to_send->user.username, account);

    data_from_server = communicate(connection, data_to_send);

    return data_from_server;
}

void expUpC(Character character, char * account, Connection * connection) {

    Data * data_to_send = newData(EXP_UP);
    Data * data_from_server;

    strcpy(data_to_send->character.name, character.name);
    data_to_send->character.lvl = character.lvl;
    data_to_send->character.totalExp = character.totalExp;
    data_to_send->character.currentExp = character.currentExp;

    strcpy(data_to_send->user.username, account);

    data_from_server = communicate(connection, data_to_send);

    free(data_from_server);

    return;
  
}

void quitC(char * account, Connection * connection, int logout) {
    
    Data * data_to_send;
    Data * data_from_server;

    if(logout == 1) {

        data_to_send = newData(EXIT_AND_LOGOUT);

        strcpy(data_to_send->user.username, account);

        data_from_server = communicate(connection, data_to_send);

        free(data_from_server);

    } else {

        data_to_send = newData(EXIT);
        sendData(connection, data_to_send);

    }



    return;
}

void logoutC(char * account, Connection * connection) {
    
    Data * data_to_send = newData(LOGOUT);
    Data * data_from_server;

    strcpy(data_to_send->user.username, account);

    data_from_server = communicate(connection, data_to_send);

    free(data_from_server);

    return;
}

Data * communicate(Connection * conn, Data * tosend) {
    
    Data * data_received;

    if (sendData(conn, tosend) > 0) {
        if ((data_received=receiveData(conn)) != NULL) {
            return data_received;
        
        } else {
            printf("Couldn´t receive data from server\n");
            exit(1);
        }
    
    } else {
        printf("Couldn´t send data so server, check if server is avaible.\n");
        exit(1);
    }
    
}