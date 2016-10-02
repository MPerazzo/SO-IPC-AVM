#ifndef _SHAREDMEM_H
#define _SHAREDMEM_H

#include <semaphore.h>
#include "types.h"

#define INT_SIZE 20

sem_t * init(int pid);
void send(Data * data, int pid);
Data * recive(int pid, sem_t * sem);
void deleteShared(int pid, sem_t * sem);

#endif
