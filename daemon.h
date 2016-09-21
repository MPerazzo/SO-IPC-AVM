#ifndef _DATAGRAM_H
#define _DATAGRAM_H

#define MAXM_LEN 50
#define INFO_TYPE 1
#define WARNING_TYPE 2
#define ERROR_TYPE 3
#define MESSAGE_TYPES 3

typedef enum { false, true } bool;

typedef struct
{
	long mtype;
	char mdata[MAXM_LEN];
	int svpid;
}
Message;

#endif
