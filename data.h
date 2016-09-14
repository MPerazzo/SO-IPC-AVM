#define MAXM_LEN 6

typedef enum { false, true } bool;

/* data related to AVM application */
typedef struct {

	int code;

	/* response of server */
	char message[MAXM_LEN];

	/* testing data */
	int number;
} AVMData;

typedef struct {
	int size;
	int opcode;
	int client_pid;
	AVMData avmdata;
} Data;

typedef struct {
	char * arguments;
} Datagram;

typedef struct {
	int sender_pid;
} Connection;
