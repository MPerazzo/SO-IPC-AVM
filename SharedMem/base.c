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

Data * data_from_sv;
Data * data_to_send;
int main() {
	sem_t * sem = init(0);
	while(1) {

		data_from_sv = recive(0, sem);

		int newpid = fork();

		if(newpid == 0) {
			sleep(2);
			printf("[server%d]: %s\n",data_from_sv->sender_pid,data_from_sv->avmdata.message);
			data_to_send = malloc(sizeof(Data));
			data_to_send->sender_pid = 0;
			strcpy(data_to_send->avmdata.message, "hola server");
			send(data_to_send, data_from_sv->sender_pid);
			deleteShared(0, sem);
			return 0;
		}

	}
	return 0;
}

