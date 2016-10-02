#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h> 
#include <sys/mman.h>
#include <semaphore.h>
#include <stdlib.h>
#include <string.h>
#include "sharedMem.h"

void resetSem(sem_t * sem);

sem_t * init(int pid) {
	sem_t * sem;
	int fd;
	char name[INT_SIZE];
	void * addr;
	sprintf(name, "%d", pid);

	if((fd = shm_open(name ,O_CREAT | O_RDWR ,S_IRUSR | S_IWUSR)) == -1) {
		printf("shm_open failed\n");
		perror("Error");
		exit(1);
	}
	printf("shm_open %s\n",name);

	ftruncate(fd, sizeof(Data));
	addr = mmap (NULL , sizeof(Data), PROT_READ | PROT_WRITE, MAP_SHARED , fd, 0);
	if(!addr) {
		printf("mmap failed\n");
		perror("Error");
		exit(1);
	}

	close(fd);

	sprintf(name, "sem%d", pid);
	if((sem = sem_open(name, O_CREAT, S_IRUSR | S_IWUSR, 0)) == SEM_FAILED) {
		printf("sem_open failed\n");
		perror("Error");
		exit(1);
	}
	printf("sem_open %s\n",name);
	return sem;
}

void send(Data * data, int pid) {

	int fd;
	void * addr;
	char name[INT_SIZE];
	sprintf(name, "%d", pid);

	if((fd = shm_open(name ,O_RDWR ,S_IRUSR | S_IWUSR)) == -1) {
		printf("shm_open failed\n");
		perror("Error");
		exit(1);
	}
	printf("shm_open %s\n",name);

	ftruncate(fd, sizeof(Data));
	addr = mmap (NULL , sizeof(Data), PROT_READ | PROT_WRITE, MAP_SHARED , fd, 0);
	if(!addr) {
		printf("mmap failed\n");		
		perror("Error");
		exit(1);
	}

	close(fd);

	memcpy(addr, data, sizeof(Data));

	sprintf(name, "sem%d", pid);
	sem_post(sem_open(name, O_RDWR));
	printf("sem_open and sem_post %s\n",name);

}

void resetSem(sem_t * sem) {
	int aux = 0;
	sem_getvalue(sem,&aux);
	while(aux > 0) {
		sem_wait(sem);
		sem_getvalue(sem,&aux);
	}
}

Data * recive(int pid, sem_t *sem) {

	Data * data = malloc(sizeof(Data));
	int fd;
	void * addr;
	char name[INT_SIZE];
	sprintf(name, "%d", pid);

	resetSem(sem);
	sem_wait(sem);

	printf("sem_wait sem%d\n",pid);

	if((fd = shm_open(name, O_RDONLY, S_IRUSR | S_IWUSR)) == -1) {
		printf("shm_open failed\n");
		perror("Error");
		return NULL;
	}
	printf("shm_open %s\n",name);

	ftruncate(fd, sizeof(Data));
	addr = mmap (NULL , sizeof(Data), PROT_READ, MAP_SHARED , fd, 0);
	if(!addr) {
		printf("mmap failed\n");		
		perror("Error");
		return NULL;
	}
	close(fd);

 	memcpy(data, addr, sizeof(Data));

return data;
}

void deleteShared(int pid, sem_t * sem) {

	char name[INT_SIZE];
	sprintf(name, "%d", pid);

	sem_close(sem);
	munmap(sem, sizeof(Data));
	unlink(name);
}