/* As marshalling and unmarshalling use memcpy: 
1 - Structure packing flag will be set to zero on the data structures
2 - There won´t be pointers on the data structures 
3 - Client and server should be executed by CPU´s of same arquitecture.
*/

#ifndef _DATAGRAM_H
#define _DATAGRAM_H

#define MSG_LEN 20

typedef enum { false, true } bool;

/* data related to AVM application */
typedef struct __attribute__((__packed__)) {

	int code;

	/* response of server */
	char message[MSG_LEN];

	/* testing data */
	int number;
} AVMData;

typedef struct __attribute__((__packed__)) {
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
