#define MAXM_LEN 50

void _clearmsg();

typedef enum { false, true } bool;

typedef struct
{
	long mtype;
	char mdata[MAXM_LEN];
	int svpid;
}
Message;
