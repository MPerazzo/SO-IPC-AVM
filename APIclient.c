#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "types.h"
#include "constants.h"
#include "comm.h"

Data * newData(Opcode);
Data * communicate(Connection *, Data *);
Data * receiveData(Connection * connection);

Data * loginC(char * account,char * password, Connection * connection) {

    Data * data_from_server;
    Data * data_to_send = newData(LOGIN);

    strcpy(data_to_send->user.username, account);
    strcpy(data_to_send->user.password, password);

    data_from_server = communicate(connection, data_to_send);

    free(data_to_send);

    return data_from_server;
}

Data * createAccountC(char * account, char * password, Connection * connection) {

    Data * data_from_server;
    Data * data_to_send = newData(CREATE_ACCOUNT);

    strcpy(data_to_send->user.username, account);
    strcpy(data_to_send->user.password, password);

    data_from_server = communicate(connection, data_to_send);

    free(data_to_send);

    return data_from_server;
}

Data * selectCharacterC(char * name, char * account, Connection * connection) {

    Data * data_from_server;
    Data * data_to_send = newData(SELECT_CHARACTER);

    strcpy(data_to_send->character.name, name);
    strcpy(data_to_send->user.username, account);

    data_from_server = communicate(connection, data_to_send);

    free(data_to_send);

    return data_from_server;
}

Data * createCharacterC(char * name, char * account, Connection * connection) {

    Data * data_from_server;
    Data * data_to_send = newData(CREATE_CHARACTER);

    strcpy(data_to_send->character.name, name);
    strcpy(data_to_send->user.username, account);

    data_from_server = communicate(connection, data_to_send);

    free(data_to_send);

    return data_from_server;
}

Data * deleteCharacterC(char * name, char * account, Connection * connection) {

    Data * data_from_server;
    Data * data_to_send = newData(DELETE_CHARACTER);

    strcpy(data_to_send->character.name, name);
    strcpy(data_to_send->user.username, account);

    data_from_server = communicate(connection, data_to_send);

    free(data_to_send);

    return data_from_server;
}

Data * showCharactersC(char * account, Connection * connection) {

    Data * data_from_server;
    Data * data_to_send = newData(SHOW_CHARACTER);

    strcpy(data_to_send->user.username, account);

    data_from_server = communicate(connection, data_to_send);

    free(data_to_send);

    return data_from_server;
}

void expUpC(Character character, char * account, Connection * connection) {

    Data * data_to_send = newData(EXP_UP);

    strcpy(data_to_send->character.name, character.name);
    data_to_send->character.lvl = character.lvl;
    data_to_send->character.totalExp = character.totalExp;
    data_to_send->character.currentExp = character.currentExp;

    strcpy(data_to_send->user.username, account);

	communicate(connection, data_to_send);

    free(data_to_send);

    return;
  
}

void quitC(char * account, Connection * connection, bool logout) {
    
    Data * data_to_send;

    if(logout == true) {

        data_to_send = newData(EXIT_AND_LOGOUT);

        strcpy(data_to_send->user.username, account);

        communicate(connection, data_to_send);


    } else {

        data_to_send = newData(EXIT);
        sendData(connection, data_to_send);

    }

    free(data_to_send);
}

void logoutC(char * account, Connection * connection) {
    
    Data * data_to_send = newData(LOGOUT);

    strcpy(data_to_send->user.username, account);

    communicate(connection, data_to_send);

    free(data_to_send);
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