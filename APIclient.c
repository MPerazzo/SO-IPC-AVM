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
Data * selectCharacterC(char *, Connection *);
Data * createCharacterC(char *, Connection *);
Data * deleteCharacterC(char *, Connection *);
Data * showCharactersC(Connection *);
void saveStatsC(Character, Connection *);
Data * communicate(Connection *, Data *);


Data * loginC(char * account,char * password, Connection * connection) {

    Data * data_from_server;
    Data * data_to_send = newData(LOGIN);

    strcpy(data_to_send->avmdata.user.account, account);
    strcpy(data_to_send->avmdata.user.password, password);

    sendData(connection, data_to_send);

    data_from_server = communicate(connection, data_to_send);
    return data_from_server;
}

Data * createAccountC(char * account, char * password, Connection * connection) {

    Data * data_from_server;
    Data * data_to_send = newData(CREATE_ACCOUNT);

    strcpy(data_to_send->avmdata.user.account, account);
    strcpy(data_to_send->avmdata.user.password, password);

    data_from_server = communicate(connection, data_to_send);

    free(data_to_send);

    return data_from_server;
}

Data * selectCharacterC(char * name, Connection * connection) {

    Data * data_from_server;
    Data * data_to_send = newData(SELECT_CHARACTER);

    strcpy(data_to_send->avmdata.charSelected.name, name);

    sendData(connection, data_to_send);

    data_from_server = receiveData(connection);

    free(data_to_send);

    return data_from_server;
}

Data * createCharacterC(char * name, Connection * connection) {

    Data * data_from_server;
    Data * data_to_send = newData(CREATE_CHARACTER);

    strcpy(data_to_send->avmdata.charSelected.name, name);

    data_from_server = communicate(connection, data_to_send);

    free(data_to_send);

    return data_from_server;
}

Data * deleteCharacterC(char * name, Connection * connection) {

    Data * data_from_server;
    Data * data_to_send = newData(DELETE_CHARACTER);

    strcpy(data_to_send->avmdata.charSelected.name, name);

    data_from_server = communicate(connection, data_to_send);

    free(data_to_send);

    return data_from_server;
}

Data * showCharactersC(Connection * connection) {

    Data * data_from_server;
    Data * data_to_send = newData(SHOW_CHARACTER);

    data_from_server = communicate(connection, data_to_send);

    free(data_to_send);

    return data_from_server;
}

void saveStatsC(Character character, Connection * connection) {

        Data * data_to_send = newData(SAVE_STATS);

        strcpy(data_to_send->avmdata.charSelected.name, character.name);
        data_to_send->avmdata.charSelected.lvl = character.lvl;
        data_to_send->avmdata.charSelected.totalExp = character.totalExp;
        data_to_send->avmdata.charSelected.currentExp = character.currentExp;

        sendData(connection, data_to_send);   
  
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