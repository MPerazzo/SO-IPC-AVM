#ifndef _TYPES_H
#define _TYPES_H

#include "constants.h"

typedef enum { false, true } bool;

typedef int Opcode;

typedef struct __attribute__((__packed__)) {
		char name[SIZE];
		int lvl;
		int totalExp;
		int currentExp;
} Character;

typedef struct __attribute__((__packed__)) {
		char username[SIZE];
		char password[SIZE];
} User;

typedef struct __attribute__((__packed__)) {
	int sender_pid;
	Opcode opcode;
	Character characters[MAX_CHARACTERS];
	Character character;
	int cantChars;
	User user;
} Data;

typedef struct __attribute__((__packed__)) {
	char * payload;
} Datagram;

#endif
