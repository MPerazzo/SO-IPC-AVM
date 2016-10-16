
#ifndef _CONSTANTS_H
#define _CONSTANTS_H

enum explvls { lvl1=10, lvl2=20, lvl3=40, lvl4=80, lvl5=160, lvl6=340, lvl7=680, lvl8=1360, lvl9=2720, lvl10=5440 };

/*Game States*/
#define USER_LOGIN 0
#define CHAR_SELECTION 1
#define PLAY_GAME 2
#define EXIT_GAME 3

/*Opcodes */
#define LOGIN 0
#define CREATE_ACCOUNT 1
#define SELECT_CHARACTER 2
#define CREATE_CHARACTER 3
#define DELETE_CHARACTER 4
#define SHOW_CHARACTER 5
#define EXP_UP 14

#define CHAR_ALREADY_EXISTS 20
#define CHAR_DOESNT_EXIST 21
#define CHAR_SELECT_SUCCESFUL 22
#define CHAR_BUSY 23
#define USER_DOESNT_EXIST 24
#define USER_BUSY 25
#define USER_ALREADY_EXISTS 26
#define TOO_MANY_CHARS 27
#define NO_CHARS 28
#define NO_ERROR 500

#define EXIT 200
#define EXIT_AND_LOGOUT 300
#define LOGOUT 400

#define CONNECTION_INTERRUMPED 555

/*lengths, sizes and others*/
#define CANT_ARGS 5
#define COMM_SIZE 11
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
#define QUERY_SIZE 200
#define FIRST_SESSION 1
#define FOREVER 65535

#endif
