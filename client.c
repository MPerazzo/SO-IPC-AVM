#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

#include "types.h"
#include "comm.h"
#include "constants.h"

void loadCommands();
void parser(char * , int);
int splitArgs(char * , char * args[], int);
char * strCpy(char * );

void login(char *, char *);
void createAccount(char *, char *);
void selectCharacter(char *);
void createCharacter(char *);
void deleteCharacter(char *);
void showCharacters();
void point();
void quit();
void logOut();
void help();

Data * loginC(char *,char *, Connection *);
Data * createAccountC(char *, char *, Connection *);
Data * selectCharacterC(char *, char *, Connection *);
Data * createCharacterC(char *, char *, Connection *);
Data * deleteCharacterC(char *, char *, Connection *);
Data * showCharactersC(char *, Connection *);
void expUpC(Character, char *, Connection *);
void quitC(char *, Connection *, int);
void logoutC(char *, Connection *);

void run_session();
char * getaddress(char *);
void clt_sigRutine(int);

Data * newData(Opcode opcode);
Data * receiveData(Connection *);

typedef void (* func) ();

struct command {
    char * name;
    func function;
    int cantArgs;
    int actionOnState[STATES];
    char description[BUFFERSIZE];
};

struct command commands[COMM_SIZE] = {};

int levelsExp[LEVEL_MAX] = {lvl1, lvl2, lvl3, lvl4, lvl5, lvl6, lvl7, lvl8, lvl9, lvl10}; // enum in constants.h 	
int session_state;
int counter;

Character character;
char username[SIZE];
char user_input[BUFFERSIZE];

bool connected;
char * address;
Connection * connection;
Data * data_from_server;

void main(int argc, char *argv[]) {

	connected=false;

    signal(SIGINT, clt_sigRutine);

    address = getaddress("SV");

    printf("[client] connecting to server on address %s\n", address);

    connection = comm_connect(address);

     if (connection == NULL) {
     	printf("Couldn´t connect to server!\n");
        exit(1);
     }

    connected=true;

    printf("[client] connection established, write help to see commands.\n");

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
        if(session_state == EXIT_GAME) {
            comm_disconnect(connection);
            return;
        }

    }

}

void loadCommands() {

    commands[0].name = "login";
    commands[0].function = (func)&login;
    commands[0].cantArgs = 2;
    commands[0].actionOnState[USER_LOGIN] = 1;
    strcpy(commands[0].description, "Parameters: [STRING]account [STRING]password\nDescription: In order to play, you need to login.\n");

    commands[1].name = "createaccount";
    commands[1].function = (func)&createAccount;
    commands[1].cantArgs = 2;
    commands[1].actionOnState[USER_LOGIN] = 1;
    strcpy(commands[1].description, "Parameters: [STRING]account [STRING]password\nDescription: You can create an account for the game.\n");
    
    commands[2].name = "selectchar";
    commands[2].function = (func)&selectCharacter;
    commands[2].cantArgs = 1;
    commands[2].actionOnState[CHAR_SELECTION] = 1;
    strcpy(commands[2].description, "Parameters: [STRING]name\nDescription: Select one of your characters to play.\n");

    commands[3].name = "createchar";
    commands[3].function = (func)&createCharacter;
    commands[3].cantArgs = 1;
    commands[3].actionOnState[CHAR_SELECTION] = 1;
    strcpy(commands[3].description, "Parameters: [STRING]name\nDescription: Creates a character with that name.\n");

    commands[4].name = "deletechar";
    commands[4].function = (func)&deleteCharacter;
    commands[4].cantArgs = 1;
    commands[4].actionOnState[CHAR_SELECTION] = 1;
    strcpy(commands[4].description, "Parameters: [STRING]name\nDescription: Deletes the character that matches the name.\n");
    
    commands[5].name = "showchar";
    commands[5].function = (func)&showCharacters;
    commands[5].cantArgs = 0;
    commands[5].actionOnState[CHAR_SELECTION] = 1;
    strcpy(commands[5].description, "No parameters\nDescription: Show all your characters.\n");

    commands[6].name = "p";
    commands[6].function = (func)&point;
    commands[6].cantArgs = 0;
    commands[6].actionOnState[PLAY_GAME] = 1;
    strcpy(commands[6].description, "No parameters\nDescription: generates an experince point.\n");

    commands[7].name = "logout";
    commands[7].function = (func)&logOut;
    commands[7].cantArgs = 0;
    commands[7].actionOnState[CHAR_SELECTION] = 1;
    commands[7].actionOnState[PLAY_GAME] = 1;
    strcpy(commands[7].description, "No parameters\nDescription: Back to the lobby. There you can create an account, login, and others.\n");

    commands[8].name = "quit";
    commands[8].function = (func)&quit;
    commands[8].cantArgs = 0;
    commands[8].actionOnState[USER_LOGIN] = 1;
    commands[8].actionOnState[CHAR_SELECTION] = 1;
    commands[8].actionOnState[PLAY_GAME] = 1;
    strcpy(commands[8].description, "No parameters\nDescription: Exit to this game.\n");

    commands[9].name = "help";
    commands[9].function = (func)&help;
    commands[9].cantArgs = 0;
    commands[9].actionOnState[USER_LOGIN] = 1;
    commands[9].actionOnState[CHAR_SELECTION] = 1;
    commands[9].actionOnState[PLAY_GAME] = 1;
    strcpy(commands[9].description, "No parameters\nDescription: Show conmmands description.\n");   

}

void parser(char * buffer, int state) {
    
    int cant = 0, i, j, flag = 0;
    char * args[CANT_ARGS];

    cant = splitArgs(buffer, args, CANT_ARGS);
    if(cant < 0) {
        printf("[client] Error: command incorrect, write help to see the different commands.\n");
        return;
    }
    for(i = 0 ; !flag && i < COMM_SIZE ; i++) {
        if(!strcmp(args[0], commands[i].name)) {
            flag = 1;
            if(!commands[i].actionOnState[state]) {
                printf("[client] Error: you can not use that command in this state of the game, write help to see the different commands.\n");
                
                return;
            }else if ((cant - 1) != commands[i].cantArgs) {
                printf("[client] Error: incorrect amount of arguments, write help to see the different commands.\n");
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
        printf("[client] Error: command incorrect, write help to see the different commands.\n");
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

void login(char * account, char * password) {

    if(strlen(account) > SIZE || strlen(password) > SIZE) {

        printf("[client] Limit of characters exceeded. The maximum is twenty for your Username or Password.\n");

        return;

    }

    data_from_server = loginC(account, password, connection);

    if(data_from_server->opcode == USER_DOESNT_EXIST) {

        printf("[client] The Username or password is incorrect.\n");

    } else if(data_from_server->opcode == USER_BUSY) {

        printf("[client] This user is already logged.\n");

    } else if(data_from_server->opcode == NO_ERROR) {

        session_state = CHAR_SELECTION;

        strcpy(username, data_from_server->user.username);

        printf("[client] You are now logged.\n");

    }

    free(data_from_server);
}

void createAccount(char * account, char * password) {
    
    if(strlen(account) > SIZE || strlen(password) > SIZE) {

        printf("[client] Limit of characters exceeded. The maximum is twenty for your Username or Password.\n");

        return;

    }

    data_from_server = createAccountC(account, password, connection);

    if(data_from_server->opcode == USER_ALREADY_EXISTS) {

        printf("[client] Username already exist.\n");

    } if(data_from_server->opcode == NO_ERROR) {

        printf("[client] Your account been created successfully.\n");
    }

    free(data_from_server);
}

void selectCharacter(char * name) {

    if(strlen(name) > SIZE) {

        printf("[client] Limit of characters exceeded. The maximum is twenty for your character name.\n");

        return;
    }

    data_from_server = selectCharacterC(name, username, connection);

    if(data_from_server->opcode == CHAR_DOESNT_EXIST){

        printf("[client] Character with that name does not exist. hint: use showchar command.\n");

    } else if(data_from_server->opcode == CHAR_BUSY) {

        printf("[client] Character is being played with by another client\n");

    } else if(data_from_server->opcode == NO_ERROR) {

        session_state = PLAY_GAME;

        strcpy(character.name, data_from_server->character.name);
        character.lvl = data_from_server->character.lvl;
        character.totalExp = data_from_server->character.totalExp;
        character.currentExp = data_from_server->character.currentExp;

        printf("[client] Now you are playing. If you don´t know how to play use help. ENJOY!\n");
    }

    free(data_from_server);
}

void createCharacter(char * name) {

    if(strlen(name) > SIZE) {

        printf("[client] Limit of characters exceeded. The maximum is twenty for your character name.\n");

        return;
    }

    data_from_server = createCharacterC(name, username, connection);

    if(data_from_server->opcode == CHAR_ALREADY_EXISTS){

        printf("[client] A character with that name already exists. Try with other name\n");

    } else if(data_from_server->opcode == TOO_MANY_CHARS) {

        printf("[client] You can not have more characters than five.\n");
        
    } if(data_from_server->opcode == NO_ERROR) {

        printf("[client] The character was created successfully.\n");
    }


    free(data_from_server);

}

void deleteCharacter(char * name) {

    if(strlen(name) > SIZE) {

        printf("[client] Limit of characters exceeded. The maximum is twenty for your character name.\n");

        return;
    }

    data_from_server = deleteCharacterC(name, username, connection);

    if(data_from_server->opcode == CHAR_DOESNT_EXIST){

        printf("[client] Character with that name does not exist. hint: use showcharacters command.\n");

    } else if(data_from_server->opcode == CHAR_BUSY) {

        printf("[client] Character is being played with by another client.\n");

    } else {

        printf("[client] Successfully deleted character.\n");

    }

    free(data_from_server);
}

void showCharacters() {

    /*data_from_server = showCharactersC(username, connection);

    if(data_from_server->opcode == NO_ERROR) {

        int i;

        for(i = 0 ; i < data_from_server->cantChars ; i++) {

        printf("==============================================\n");

        printf("name: %s\tlevel: %d\t exp: %d/%d\n",data_from_server->characters[i].name, data_from_server->characters[i].lvl, 
            data_from_server->characters[i].currentExp, data_from_server->characters[i].totalExp);

        printf("==============================================\n");

        }
    } else {

        printf("[client] You do not have characters\n");

    }

    free(data_from_server);*/
}

void point() {
    
    if(character.currentExp++ == character.totalExp) {

        if(character.lvl < LEVEL_MAX) {

            character.totalExp = levelsExp[character.lvl];
            character.lvl ++;
            character.currentExp = 0;

            printf("[client] Congrats now %s is level %d \n",character.name, character.lvl);

        } else {

            printf("[client] You are lvl MAX already. Go and fullfill your destiny\n");

            return;
        }

    }

    printf("Your exp is %d\n", character.currentExp);

    printf("Your level is %d\n", character.lvl);

    printf("Your totalExp is %d\n", character.totalExp);

    // Save character data in database in case connection is lost
    if(counter == LIMIT_TOSAVE) {
        
        counter = 0;

        expUpC(character, username, connection);   

    } else {

        counter++;

    }
}

void quit() {

    if(session_state == USER_LOGIN) {

        quitC(username, connection, 0);

    } else if(session_state == CHAR_SELECTION) {

        quitC(username, connection, 1);

    } else if(session_state == PLAY_GAME) {

        expUpC(character, username, connection);
        quitC(username, connection, 1);

    }

    session_state = EXIT_GAME;

    printf("[client] Thank you for playing. See you later :)\n");

    return ;
}

void logOut() {

    if(session_state == CHAR_SELECTION) {

        logoutC(username, connection);

    } else if(session_state == PLAY_GAME) {

        expUpC(character, username, connection);
        logoutC(username, connection);

    }

    printf("[client] You have logged out successfully\n");

    session_state = USER_LOGIN;
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

void client_close() {
    if (connected) {
        comm_disconnect(connection);
        free(connection);
    }
}

void clt_sigRutine(int sig) {

    Data * new_data = newData(CONNECTION_INTERRUMPED);
    
    if (sendData(connection, new_data)<0)
    	printf("WTF\n");
    
    client_close();

    free(new_data);
    
    printf("\n");
    printf("Client proccess with pid: %d terminated\n", getpid());
    exit(1); 
}