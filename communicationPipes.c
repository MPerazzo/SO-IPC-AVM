#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "data.h"
#include <signal.h>

#define PUBLIC_FIFO "/tmp/server_public_fifo"


bool is_server;

int toserver_fd, toclient_fd;
char pipename[32];

void srv_initChannel();
void clt_initChannel();
int srv_sendData(Connection *, Datagram *);
int clt_sendData(Connection *, Datagram *);
void srv_receiveData(Connection *, Datagram *);
void clt_receiveData(Connection *, Datagram *);


void initChannel(int bool_server) {

    switch (bool_server) {

    case true:
        srv_initChannel();
        break;

    case false:
        clt_initChannel();
        break;
    }
}

int sendData(Connection * connection, Datagram * params) {
    switch (is_server) {
    case true:
        return srv_sendData(connection, params);
        break;

    case false:
        return clt_sendData(connection, params);
        break;
    }
}

void receiveData(Connection * connection, Datagram * params) {
    switch (is_server) {
    case true:
        srv_receiveData(connection, params);
        break;

    case false:
        clt_receiveData(connection, params);
        break;
    }
}

void srv_initChannel(void) {
    
    is_server=true;

    mknod(PUBLIC_FIFO, S_IFIFO | 0666, 0);

}


void clt_initChannel(void) {

    is_server=false;

}

/* la data tiene que traer el pid del cliente en data.client_pid para que el server pueda responder */
void srv_receiveData(Connection * connection, Datagram * buffer) {

    toserver_fd = open(PUBLIC_FIFO, O_RDONLY);

    buffer->arguments = calloc(sizeof(Data), 1);

    read(toserver_fd, buffer->arguments, sizeof(Data));

    close(toserver_fd);

}

/* Hay que pasar el client pid en connection.sender_pid para que sepa donde mandar la respuesta */
int srv_sendData(Connection * connection, Datagram * params) {

    sprintf(pipename, "/tmp/client_fifo_%d", connection->sender_pid);   
    
    /* crea una pipe temporaria*/
    mknod(pipename, S_IFIFO | 0666, 0);

    /* signal al cliente para que abra la pipe */
    kill(connection->sender_pid, SIGCONT);
    
    toclient_fd = open(pipename, O_WRONLY);
    
    write(toclient_fd, params->arguments, sizeof(Data));

    close(toclient_fd);

    /* borra la pipe temporaria*/
    unlink(pipename);

    return 0;

}

void clt_receiveData(Connection * connection, Datagram * buffer) {

    sprintf(pipename, "/tmp/client_fifo_%d", getpid());

    /* espera a que el server abra una pipe con la respuesta */
    while((toclient_fd = open(pipename, O_RDONLY)) < 0) {
        //sleep(10000);
        raise(SIGSTOP);
    }

    buffer->arguments = calloc(sizeof(Data), 1);

    read(toclient_fd, buffer->arguments, sizeof(Data));

    close(toclient_fd);

}

int clt_sendData(Connection * connection, Datagram * params) {

    toserver_fd = open(PUBLIC_FIFO, O_WRONLY);

    write(toserver_fd, params->arguments, sizeof(Data)); 

    close(toserver_fd);

    return 0;

}