/*
mtype:
		1-Info
		2-Warning
		3-Error
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "daemon.h"
#include "types.h"

void _clearmsg();

int qmessage_in, qmessage_out;
bool is_dSrv;
key_t k_in, k_out;

Message msg;

int initLogin(bool is_daemonSrv) {

	is_dSrv = is_daemonSrv;

	switch (is_dSrv) {

	case true: {
		return logsv_initLogin();
	}
		

	case false:
		return other_initLogin();
	}
}

int logsv_initLogin() {

	k_in = 0xBBBB0;
	k_out = 0XBBBB1;

	return create_queue();
}

int other_initLogin() {

	k_in = 0XBBBB1;
	k_out = 0xBBBB0;

	return create_queue();
}


int create_queue() {

	if ( (qmessage_in =  msgget(k_in, 0666 | IPC_CREAT)) == -1 )
		return -1;

	if ( (qmessage_out =  msgget(k_out, 0666 | IPC_CREAT)) == -1 )
		return -1;

	return 0;
}

int sndMessage(char message[], int type) {
	
	msg.mtype = type;
	msg.svpid = getpid();
	strcpy(msg.mdata, message);
	
	if (msgsnd(qmessage_out, &msg, sizeof(msg), 0)<0) {
		printf("sndMessage to %d queue failed" , qmessage_out);
		perror("Error:");
		_clearmsg();
		return -1;
	}
	_clearmsg();
	return 0;
}

int rcvMessage(int type)  {
	
	if (msgrcv(qmessage_in, &msg, sizeof(msg), type, 0) < 0)
	{
		printf("rcvMessage from %d queue failed", qmessage_in);
		perror("Error:"); 
		return -1;	
	}
	return 0;

}

int rcv_InfoMessage() {
	return rcvMessage(INFO_TYPE);
}

int rcv_WarningMessage() {
	return rcvMessage(WARNING_TYPE);
}

int rcv_ErrorMessage() {
	return rcvMessage(ERROR_TYPE);
}

void _clearmsg() {
 	int i;
 	for (i=0; i<MAXM_LEN; i++)
 		msg.mdata[i]=0;	
 }

char * typetoString(long type) {	
	char * strings[MESSAGE_TYPES] = {"Info", "Warning", "Error"};
	return strings[type-1];
}

 void printMessage() {
	char * string = (is_dSrv==0)?"Deamon":"Server";
	printf("Message from server is : %s .\nDesciption : Message type %s, received from %s pid : %d\n\n", msg.mdata, typetoString(msg.mtype), string, msg.svpid);
}
