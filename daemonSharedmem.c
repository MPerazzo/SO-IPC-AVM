/*
Unidirectional

Message.type:
		
		1-Info
		2-Warning
		3-Error
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <pthread.h>

#include "daemon.h"
#include "types.h"
#include "semaphores.h"

void dSrv_close();
void other_close();

struct Message
{
	char tosay[MAXM_LEN];
	int type;
	int svpid;
};

Message msg;

bool is_dSrv;

bool error;

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

int mutex;


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

	sem_unlink("SemDaemon");

	if ((daemon_sem = sem_open("SemDaemon", O_CREAT,  S_IRUSR | S_IWUSR, 0)) == SEM_FAILED) {
		printf("sem_open failed\n");
		perror("Error.");
		return -1;
	}

	mutex = binary_semaphore_allocation(666, IPC_RMID);

    binary_semaphore_initialize(mutex);

	shm_id = shmget(shm_key, shm_size, IPC_CREAT | SHM_W | SHM_R);

	shm_address = (char*) shmat (shm_id, NULL, 0);

	return 0;
}

int other_initLogin() {
	
	if ((daemon_sem = sem_open("SemDaemon", O_CREAT)) == SEM_FAILED) {
		error = true;
		printf("sem_open failed\n");
		perror("Error");
		return -1;
	}

	mutex = binary_semaphore_allocation (666, IPC_RMID);

	shm_id = shmget(shm_key, shm_size, IPC_CREAT | SHM_W | SHM_R);

	shm_address = (char*) shmat (shm_id, NULL, 0);

	return 0;	
}


int sndMessage(char * message, int type) {

	if (error) // to avoid segm fault if server is running before daemonServer as it would be using a sem not initialized.
		return;

	binary_semaphore_wait(mutex);

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
		binary_semaphore_post(mutex);
		return -1;
	}

	//set message data using the information retrieved

	strcpy(msg.tosay, shared_tosay);

	msg.type = atoi(shared_type);

	msg.svpid = atoi(shared_pid);

	binary_semaphore_post(mutex);

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

void close_daemon() {

switch (is_dSrv) {

	case true: {
		dSrv_close();
	}

	case false:
		other_close();
	}

}

void dSrv_close() {

    shmdt(shm_address);

    shmctl(shm_id , IPC_RMID , NULL);

	binary_semaphore_deallocate(mutex);

	sem_close(daemon_sem);

	sem_unlink("SemDaemon");

}

void other_close() {
	sem_close(daemon_sem);
}

char * typetoString(int type) {	
	char * strings[MESSAGE_TYPES] = {"Info", "Warning", "Error"};
	return strings[type-1];
}

void printMessage() {
	printf("Message from server is : %s.\nDescription : Message type %s, received from Server pid : %d\n\n", msg.tosay, typetoString(msg.type), msg.svpid);
}


