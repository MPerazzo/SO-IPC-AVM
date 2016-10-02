#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

#include "types.h"
#include "comm.h"
#include "constants.h"

void connect();
void clt_sigRutine(int);
void run_session();
void client_close();
char * getaddress();

Data * communicate(Connection * conn, Data * tosend);
Data * receiveData(Connection * connection);

void loadCommands();
void parser(char * , int);
int splitArgs(char * , char * args[], int);
char * strCpy(char * );
int convertArg(char ** , char * , int);

void login(char * ,char *);
void createAccount(char *, char *);
void selectCharacter(char *);
void createCharacter(char *);
void deleteCharacter(char *);
void showCharacters();
void exitGame();
void help();
void logOut();
void point();
void getLevel();
void freeData();

Data * newData(Opcode);

typedef void (* func) ();

struct command {
    char * name;
    func function;
    char typeArgs[CANT_ARGS];
    int cantArgs;
    int actionOnState[STATES];
    char description[BUFFERSIZE];
};

struct command commands[COMM_SIZE] = {};

char user_input[BUFFERSIZE];

int session_state;

bool connected;

Connection * connection;
Data * data_from_server;
Data * data_to_send;

Character character_in_game;
int counter = 0;
int levelsExp[LEVEL_MAX] = {10, 20, 40, 80, 160, 340, 680, 1360, 2720, 5440};

void main(int argc, char *argv[]) {

    connected=false;

    signal(SIGINT, clt_sigRutine);

    char * address = getaddress();

    printf("[client] trying to connect to server on address %s\n", address);

    connection = comm_connect(address);

    if (connection==NULL)
        exit(1);

    connected=true;

    printf("[client] connection established. Write help to see commands.\n");

    run_session();

    printf("[client] session done\n");

}


void run_session() {

    session_state = USER_LOGIN;
    loadCommands();

    while(1) {
        printf(">>");
        fgets(user_input, BUFFERSIZE, stdin);
        parser(user_input, session_state);

        if(session_state == END_SESSION) {

            sendData(connection, newData(END_OF_CONNECTION));

            client_close();

            return ;

        }

    }

}

void loadCommands() {

    commands[LOGIN].name = "login";
    commands[LOGIN].function = (func)&login;
    commands[LOGIN].typeArgs[0] = STRING;
    commands[LOGIN].typeArgs[1] = STRING;
    commands[LOGIN].cantArgs = 2;
    commands[LOGIN].actionOnState[USER_LOGIN] = 1;
    strcpy(commands[LOGIN].description, "Parameters: [STRING]account [STRING]password\nDescription: In order to play, you need to login.\n");

    commands[CREATE_ACCOUNT].name = "createAccount";
    commands[CREATE_ACCOUNT].function = (func)&createAccount;
    commands[CREATE_ACCOUNT].typeArgs[0] = STRING;
    commands[CREATE_ACCOUNT].typeArgs[1] = STRING;
    commands[CREATE_ACCOUNT].cantArgs = 2;
    commands[CREATE_ACCOUNT].actionOnState[USER_LOGIN] = 1;
    strcpy(commands[CREATE_ACCOUNT].description, "Parameters: [STRING]account [STRING]password\nDescription: You can create an account for the game.\n");
    
    commands[SELECT_CHARACTER].name = "selectCharacter";
    commands[SELECT_CHARACTER].function = (func)&selectCharacter;
    commands[SELECT_CHARACTER].typeArgs[0] = STRING;
    commands[SELECT_CHARACTER].cantArgs = 1;
    commands[SELECT_CHARACTER].actionOnState[CHAR_SELECTION] = 1;
    strcpy(commands[SELECT_CHARACTER].description, "Parameters: [STRING]name\nDescription: Select one of your characters to play.\n");

    commands[CREATE_CHARACTER].name = "createCharacter";
    commands[CREATE_CHARACTER].function = (func)&createCharacter;
    commands[CREATE_CHARACTER].typeArgs[0] = STRING;
    commands[CREATE_CHARACTER].cantArgs = 1;
    commands[CREATE_CHARACTER].actionOnState[CHAR_SELECTION] = 1;
    strcpy(commands[CREATE_CHARACTER].description, "Parameters: [STRING]name\nDescription: Creates a character with the name that you typing.\n");

    commands[DELETE_CHARACTER].name = "deleteCharacter";
    commands[DELETE_CHARACTER].function = (func)&deleteCharacter;
    commands[DELETE_CHARACTER].typeArgs[0] = STRING;
    commands[DELETE_CHARACTER].cantArgs = 1;
    commands[DELETE_CHARACTER].actionOnState[CHAR_SELECTION] = 1;
    strcpy(commands[DELETE_CHARACTER].description, "Parameters: [STRING]name\nDescription: Delete a character with the name that you typing.\n");

    commands[SHOW_CHARACTER].name = "showCharacters";
    commands[SHOW_CHARACTER].function = (func)&showCharacters;
    commands[SHOW_CHARACTER].cantArgs = 0;
    commands[SHOW_CHARACTER].actionOnState[CHAR_SELECTION] = 1;
    strcpy(commands[SHOW_CHARACTER].description, "No parameters\nDescription: Show all your characters.\n");

    commands[EXIT_GAME].name = "exitGame";
    commands[EXIT_GAME].function = (func)&exitGame;
    commands[EXIT_GAME].cantArgs = 0;
    commands[EXIT_GAME].actionOnState[USER_LOGIN] = 1;
    commands[EXIT_GAME].actionOnState[CHAR_SELECTION] = 1;
    commands[EXIT_GAME].actionOnState[PLAY_GAME] = 1;
    strcpy(commands[EXIT_GAME].description, "No parameters\nDescription: Exit to this game.\n");

    commands[HELP].name = "help";
    commands[HELP].function = (func)&help;
    commands[HELP].cantArgs = 0;
    commands[HELP].actionOnState[USER_LOGIN] = 1;
    commands[HELP].actionOnState[CHAR_SELECTION] = 1;
    commands[HELP].actionOnState[PLAY_GAME] = 1;
    strcpy(commands[HELP].description, "No parameters\nDescription: Show conmmands description.\n");

    commands[LOGOUT].name = "logOut";
    commands[LOGOUT].function = (func)&logOut;
    commands[LOGOUT].cantArgs = 0;
    commands[LOGOUT].actionOnState[CHAR_SELECTION] = 1;
    commands[LOGOUT].actionOnState[PLAY_GAME] = 1;
    strcpy(commands[LOGOUT].description, "No parameters\nDescription: Back to the lobby. There you can create an account, login, and others.\n");

    commands[POINT].name = "p";
    commands[POINT].function = (func)&point;
    commands[POINT].cantArgs = 0;
    commands[POINT].actionOnState[PLAY_GAME] = 1;
    strcpy(commands[POINT].description, "No parameters\nDescription: generates an experince point.\n");

    commands[GET_LEVEL].name = "getLevel";
    commands[GET_LEVEL].function = (func)&getLevel;
    commands[GET_LEVEL].cantArgs = 0;
    commands[GET_LEVEL].actionOnState[PLAY_GAME] = 1;
    strcpy(commands[GET_LEVEL].description, "No parameters\nDescription: Show the level of character and the totalExp required for the next level. Also shows the currentExp.\n");
}

void parser(char * buffer, int state) {
    
    int cant = 0, i, j, flag = 0;
    char * args[CANT_ARGS];

    cant = splitArgs(buffer, args, CANT_ARGS);
    if(cant < 0) {
        printf("Error: command incorrect, write help to see the different commands.\n");
        return;
    }
    for(i = 0 ; !flag && i < COMM_SIZE ; i++) {

        if(!strcmp(args[0], commands[i].name)) {
            flag = 1;
            if(!commands[i].actionOnState[state]) {
                printf("Error: you can not use that command in this state of the game, write help to see the different commands.\n");
                
                return;
            }else if ((cant - 1) != commands[i].cantArgs) {
                printf("Error: incorrect amount of arguments, write help to see the different commands.\n");
                return;
            } else {

                switch (commands[i].cantArgs) {
                case 0:
                    commands[i].function();
                    break;
                case 1:
                    commands[i].function(args[1]);
                    break;
                case 2:
                    commands[i].function(args[1], args[2]);
                    break;
                }
            }
        }
    }
    if(!flag) {
        printf("Error: command incorrect, write help to see the different commands.\n");
    }
}

int splitArgs(char * buffer, char * args[], int cantArgs) {
    
    int i = 0, index = 0, size = 0;

    if(buffer[i] == ' ') {
        return -1;
    }
    args[index] = strCpy(buffer);
    if(args[index] == NULL) {
        return -1;
    }
    size = strlen(args[index]);
    index++;
    i += size;
    while(buffer[i] != 0 && buffer[i] != '\n') {
        if(cantArgs == index) {
            return -1;
        }
        args[index] = strCpy((buffer + i + 1));
        if(args[index] == NULL) {
            return -1;
        }
        size = strlen(args[index]);
        index ++;
        i += size + 1;
    }
    return index;
}

char * strCpy(char * str) {
    
    int i = 0, j = 0;
    char *aux = (char *)malloc(1*sizeof(char));
    if(str == NULL) {
        return NULL;
    }

    while(str[i] != 0 && str[i] != ' ' && str[i] != '\n') {
        if(i % BLOCK == 0) {
            j++;
            aux = (char *)realloc(aux, j*BLOCK*sizeof(char));
            if(aux == NULL) {
                return NULL;
            }
        }
        aux[i] = str[i];
        i++;
    }
    aux = (char *)realloc(aux, (i+1)*sizeof(char));
    if(aux == NULL) {
        return NULL;
    }
    aux[i] = 0;
    return aux;
}

void help() {
    int i;

    printf("\n");
    printf("Sintaxis to call a function is: name_function arg1 arg2 ... argN\n");
    printf("==============================================\n");

    for (i = 0; i < COMM_SIZE; i++) {
        if(commands[i].actionOnState[session_state]){
            printf("==============================================\n");
            printf("%s:\n", commands[i].name);
            printf("%s", commands[i].description);
            printf("==============================================\n");
        }
    }
}

void login(char * account,char * password) {

    if(strlen(account) > SIZE || strlen(password) > SIZE) {
        printf("Limit of characters exceeded. The maximum is twenty for your Username or Password.\n");
        return;
    }

    data_to_send = newData(LOGIN);

    strcpy(data_to_send->avmdata.user.account, account);
    strcpy(data_to_send->avmdata.user.password, password);

    sendData(connection, data_to_send);

    data_from_server = communicate(connection, data_to_send);

    if(data_from_server->opcode == ERR_PARAMETER) {
        printf("The Username or password is incorrect.\n");
    } else if(data_from_server->opcode == NO_ERR) {
        session_state = CHAR_SELECTION;
        printf("You are now logged.\n");
    }

    freeData();
}

void createAccount(char * account, char * password) {
    
    if(strlen(account) > SIZE || strlen(password) > SIZE) {
        printf("Limit of characters exceeded. The maximum is twenty for your Username or Password.\n");
        return;
    }

    data_to_send = newData(CREATE_ACCOUNT);

    strcpy(data_to_send->avmdata.user.account, account);
    strcpy(data_to_send->avmdata.user.password, password);

    data_from_server = communicate(connection, data_to_send);

    if(data_from_server->opcode == ERR_PARAMETER){
        printf("Username already exist.\n");
    } if(data_from_server->opcode == NO_ERR) {
        printf("Your account been created successfully.\n");
    }

    freeData();
}

void selectCharacter(char * name) {

    if(strlen(name) > SIZE) {
        printf("Limit of characters exceeded. The maximum is twenty for your character name.\n");
        return;
    }

    data_to_send = newData(SELECT_CHARACTER);

    strcpy(data_to_send->avmdata.charSelected.name, name);

    sendData(connection, data_to_send);



    data_from_server = receiveData(connection);

    if(data_from_server->opcode == ERR_PARAMETER){
        printf("No character exists with that name.\n");
    } if(data_from_server->opcode == NO_ERR) {
        session_state = PLAY_GAME;
        strcpy(character_in_game.name, data_from_server->avmdata.charSelected.name);
        character_in_game.lvl = data_from_server->avmdata.charSelected.lvl;
        character_in_game.totalExp = data_from_server->avmdata.charSelected.totalExp;
        character_in_game.currentExp = data_from_server->avmdata.charSelected.currentExp;

        printf("Now you are playing. If you don´t know how to play use help. ENJOY!\n");
    }

    freeData();
}

void createCharacter(char * name) {

    if(strlen(name) > SIZE) {
        printf("Limit of characters exceeded. The maximum is twenty for your character name.\n");
        return;
    }
    
    data_to_send = newData(CREATE_CHARACTER);

    strcpy(data_to_send->avmdata.charSelected.name, name);

    data_from_server = communicate(connection, data_to_send);

    if(data_from_server->opcode == ERR_PARAMETER){
        if(data_from_server->avmdata.cantCharacters == MAX_CHARACTERS) {
            printf("You can not have more characters than 5\n");
        } else {
            printf("That name already exists.\n");
        }
    } if(data_from_server->opcode == NO_ERR) {
        printf("The character was created successfully.\n");
    }

    freeData();
}

void deleteCharacter(char * name) {
    if(strlen(name) > SIZE) {
        printf("Limit of characters exceeded. The maximum is twenty for your character name.\n");
        return;
    }

    data_to_send = newData(DELETE_CHARACTER);

    strcpy(data_to_send->avmdata.charSelected.name, name);

    data_from_server = communicate(connection, data_to_send);

    if(data_from_server->opcode == ERR_PARAMETER){
        printf("No character exists with that name.\n");
    } if(data_from_server->opcode == NO_ERR) {
        printf("The character %s has been deleted.\n", name);
    }

    freeData();
}

void showCharacters() {

    data_to_send = newData(SHOW_CHARACTER);

    data_from_server = communicate(connection, data_to_send);

    if(data_from_server->avmdata.cantCharacters != 0) {
        int i;
        for(i = 0 ; i < MAX_CHARACTERS ; i++) {
        printf("==============================================\n");
        printf("name: %s\tlevel: %d\t exp: %d/%d\n",data_from_server->avmdata.characters[i].name, data_from_server->avmdata.characters[i].lvl, 
            data_from_server->avmdata.characters[i].currentExp, data_from_server->avmdata.characters[i].totalExp);
        printf("==============================================\n");
        }
    } else {
        printf("You do not have characters\n");
    }

    freeData();
}

void point() {

    printf("Your exp is %d\n", character_in_game.currentExp);

    printf("Your level is %d\n", character_in_game.lvl);

    printf("Your totalExp is %d\n", character_in_game.totalExp);
    
    if(character_in_game.currentExp++ == character_in_game.totalExp) {
        if(character_in_game.lvl < LEVEL_MAX) {

            character_in_game.lvl ++;
            character_in_game.totalExp = levelsExp[character_in_game.lvl];
            character_in_game.currentExp = 0;
            printf("Congrats now %s is level %d \n",character_in_game.name, character_in_game.lvl);
        } else {

            printf("You are lvl MAX already. Go and fullfill your destiny\n");
            return;
        }

    }
    // Save character data in database in case connection is lost
    if(counter == LIMIT_TOSAVE) {
        
        counter = 0;

        data_to_send = newData(SAVE_STATS);

        strcpy(data_to_send->avmdata.charSelected.name, character_in_game.name);
        data_to_send->avmdata.charSelected.lvl = character_in_game.lvl;
        data_to_send->avmdata.charSelected.totalExp = character_in_game.totalExp;
        data_to_send->avmdata.charSelected.currentExp = character_in_game.currentExp;

        sendData(connection, data_to_send);   

        free(data_to_send);     

    } else {
        counter++;
    }
}

void getLevel() {
     printf("level: %d\t exp: %d/%d\n",character_in_game.lvl, character_in_game.currentExp, character_in_game.totalExp);
}

void exitGame() {
    
    if(session_state == PLAY_GAME) {
        
        data_to_send = newData(EXIT_GAME_SAVE);

        sendData(connection, data_to_send);

    } else {
        
        data_to_send = newData(EXIT_GAME);

        sendData(connection, data_to_send);; 
    }
    printf("Thank you for playing. See you later :)\n");

    free(data_to_send);

    session_state = END_SESSION;
}

void logOut() {

    if(session_state == PLAY_GAME) {
        
        data_to_send = newData(LOGOUT_SAVE);

        sendData(connection, data_to_send);

    } else {
        
        data_to_send = newData(LOGOUT);

        sendData(connection, data_to_send);
    }
    printf("You have logged out successfully\n");

    free(data_to_send);

    session_state = USER_LOGIN;
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

void client_close() {
    if (connected) {
        comm_disconnect(connection);
        free(connection);
    }
}

void clt_sigRutine(int sig) {

    Data * new_data = newData(CONNECTION_INTERRUMPED);
    
    sendData(connection, new_data);
    
    client_close();

    free(new_data);
    
    printf("\n");
    printf("Client proccess with pid: %d terminated\n", getpid());
    exit(1); 
}

void freeData() {

    free(data_to_send);
    free(data_from_server);
}

