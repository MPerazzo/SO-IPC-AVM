#include "types.h"

#ifndef _COMMUNICATION_H
#define _COMMUNICATION_H

void disConnect(Connection *);
int sendData(Connection *, Datagram *);
int receiveData(Connection *, Datagram *);
int acceptConnection(Connection *);
Connection * initChannel(int);
Connection * createConnection(int);


#endif
