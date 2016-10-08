/*
Unidirectional

Message.type:
		
		1-Info
		2-Warning
		3-Error
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>

#include "daemon.h"
#include "types.h"
#include "semaphores.h"


struct Message
{
	char tosay[MAXM_LEN];
	int type;
	int svpid;
};

Message msg;

bool is_dSrv;

// sharedMemory variables

const int shm_size = 1024;

key_t shm_key;

int shm_id;

char * shm_address;

char * shm_position;

int args_size[DAEMON_CANT_ARGS-1];

// semaphore

sem_t * daemon_sem;

// mutex

pthread_mutex_t * mutex = NULL;

pthread_mutexattr_t attrmutex;


int initLogin(bool is_daemonSrv) {

	is_dSrv = is_daemonSrv;

	shm_key = getkey("SHM");

	switch (is_dSrv) {

	case true: {
		return dSrv_initLogin();
	}
		

	case false:
		return other_initLogin();
	}
}

int dSrv_initLogin() {

	if ((daemon_sem = sem_open("mySemprobando", O_CREAT,  S_IRUSR | S_IWUSR, 1)) == SEM_FAILED) {
		printf("sem_open failed\n");
		perror("Error.");
		return -1;
	}

	printf("WTF1\n");

	pthread_mutexattr_init(&attrmutex);

	printf("WTF2\n");
	
	pthread_mutexattr_setpshared(&attrmutex, PTHREAD_PROCESS_SHARED);

	printf("WTF3\n");

	mutex = malloc(sizeof(mutex));

	pthread_mutex_init(mutex, &attrmutex);

	printf("WTF2\n");

	shm_id = shmget(shm_key, shm_size, IPC_CREAT | SHM_W | SHM_R);

	shm_address = (char*) shmat (shm_id, NULL, 0);

	return 0;
}

int other_initLogin() {

	mutex = malloc(sizeof(mutex));

	if ((daemon_sem = sem_open("mySemprobando", O_CREAT)) == SEM_FAILED) {
		printf("sem_open failed\n");
		perror("Error");
		return -1;
	}

	pthread_mutexattr_init(&attrmutex);

	pthread_mutexattr_setpshared(&attrmutex, PTHREAD_PROCESS_SHARED);

	pthread_mutex_init(mutex, &attrmutex);

	printf("WTF2\n");

	shm_id = shmget(shm_key, shm_size, IPC_CREAT | SHM_W | SHM_R);

	shm_address = (char*) shmat (shm_id, NULL, 0);

	return 0;	
}


int sndMessage(char * message, int type) {

    pthread_mutex_lock(mutex);

	shm_position = shm_address + sizeof(args_size);

	args_size[0] = sprintf(shm_position, "%s", message) +1;
	shm_position += args_size[0];

	args_size[1] = sprintf(shm_position, "%d", type) +1;
	shm_position += args_size[1];

	sprintf(shm_position, "%d", getpid());

	memcpy(shm_address, &args_size, sizeof(args_size));

	shm_position = shm_address;

	sem_post(daemon_sem);

	return 0;
}

int rcvMessage(int type) {

	sem_wait(daemon_sem);

	shm_position = shm_address;

	// retrieves information from sharedMem

	int * aux;

	aux = (int *)shm_address;

	shm_position = shm_address + sizeof(args_size);

	char * shared_tosay = shm_position;

	shm_position += *aux;

	char * shared_type = shm_position;

	shm_position += *(++aux);

	char * shared_pid = shm_position;

	if ( type != 0 && atoi(shared_type) != type) {
		pthread_mutex_unlock(mutex);
		return -1;
	}

	//set message data using the information retrieved

	strcpy(msg.tosay, shared_tosay);

	msg.type = atoi(shared_type);

	msg.svpid = atoi(shared_pid);

	pthread_mutex_unlock(mutex);

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

char * typetoString(int type) {	
	char * strings[MESSAGE_TYPES] = {"Info", "Warning", "Error"};
	return strings[type-1];
}

void printMessage() {
	printf("Message from server is : %s.\nDescription : Message type %s, received from Server pid : %d\n\n", msg.tosay, typetoString(msg.type), msg.svpid);
}


