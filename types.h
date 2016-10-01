/* As marshalling and unmarshalling use memcpy: 
1 - Structure packing flag will be set to zero on the data structures
2 - There won´t be pointers on the data structures 
3 - Client and server should be executed by CPU´s of same arquitecture.
*/

#ifndef _TYPES_H
#define _TYPES_H

#include "constants.h"

typedef int Opcode;

typedef enum { false, true } bool;

typedef struct {
		char name[SIZE];
		int lvl;
		int totalExp;
		int currentExp;
} Character;

typedef struct {
		char account[SIZE];
		char password[SIZE];
} User;

/* data related to AVM application */
typedef struct __attribute__((__packed__)) {
	Character characters[MAX_CHARACTERS];
	User user;
	int cantCharacters;
	Character charSelected;
	char message[MSG_LEN];
} AVMData;

typedef struct __attribute__((__packed__)) {
	int sender_pid;
	Opcode opcode;
	AVMData avmdata;
} Data;

typedef struct {
	char * payload;
} Datagram;

#endif
