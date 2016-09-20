#define MAXM_LEN 20

#ifndef _DATAGRAM_H
#define _DATAGRAM_H

typedef enum { false, true } bool;

/* data related to AVM application */
typedef struct {

	int code;

	/* response of server */
	char message[MAXM_LEN];

	/* testing data */
	int number;
} AVMData;

typedef struct {
	int size;
	int opcode;
	int client_pid;
	AVMData avmdata;
} Data;

typedef struct {
	char * arguments;
} Datagram;

typedef struct {
	int id;
} Connection;

#endif
