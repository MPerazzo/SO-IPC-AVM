#include "types.h"
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h> 
#include <sys/mman.h>
#include <semaphore.h>
#include <stdlib.h>
#include <string.h>
#include "sharedMem.h"

Data * data_from_db;
Data * data_to_send;

void main() {

	int pid = getpid();
	sem_t * sem = init(pid);

	data_to_send = malloc(sizeof(Data));		
	data_to_send->sender_pid = pid;
	strcpy(data_to_send->avmdata.message, "hola base");

	int i;
	for(i = 0 ; i < 5 ; i++) {
		send(data_to_send, 0);			
		sleep(2);
		data_from_db = recive(pid,sem);
		printf("[Base%d]: %s\n",data_from_db->sender_pid,data_from_db->avmdata.message);
	}

	deleteShared(pid, sem);

}