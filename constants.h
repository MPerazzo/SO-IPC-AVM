
#ifndef _CONSTANTS_H
#define _CONSTANTS_H

/*Game States*/
#define END_SESSION 0
#define USER_LOGIN 1
#define CHAR_SELECTION 2
#define PLAY_GAME 3

/*Args types*/
#define STRING 0
#define INT 1
#define DOUBLE 2

/*Opcode from client*/
#define LOGIN 0
#define CREATE_ACCOUNT 1
#define SELECT_CHARACTER 2
#define CREATE_CHARACTER 3
#define DELETE_CHARACTER 4
#define SHOW_CHARACTER 5
#define EXIT_GAME 6
#define HELP 7
#define LOGOUT 8
#define POINT 9
#define SAVE_STATS 12

/*Opcode from server*/
#define ERR_PARAMETER 1
#define NO_ERR 2

/* ocpcodes */
#define END_OF_CONNECTION 200
#define CONNECTION_INTERRUMPED 300

/*length or sizes*/
#define CANT_ARGS 5
#define COMM_SIZE 10
#define STATES 6
#define BLOCK 10
#define LEVEL_MAX 10
#define MSG_LEN 50
#define BUFFERSIZE 200
#define MAXQ_SIZE 5
#define MAX_THREADS 10
#define MAX_CHARACTERS 5
#define SIZE 20
#define KEY_LENGTH 3
#define BUFF_SIZE 20
#define LIMIT_TOSAVE 10


#endif
