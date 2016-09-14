#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include "data.h"

#define port 4020
#define server_ip "127.0.0.1"
#define MAXQSIZE 5
#define MAX_RDATA_SIZE 100



struct sockaddr_in server, client;

int request_sock; // client socket
int socket_server; // server socket
int socket_client;
int clistruct_size ;

bool is_server;

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

    socket_server = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_server == -1) {
        printf("Couldn´t create socket.\n");
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(server_ip);
    server.sin_port = htons(port);

    //This allows another socket to bind the ip, port, allows instant server reboot
    int enable = 1;
    if (setsockopt(socket_server, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
        perror("setsockopt(SO_REUSEADDR) failed");

    //Relates the socket to an address
    if (bind(socket_server, (struct sockaddr *)&server , sizeof(server)) < 0) {
        perror("bind failed. Error");
        return;
    }

    //Listen for connections from socket
    listen(socket_server, MAXQSIZE);

    clistruct_size = sizeof(struct sockaddr_in);

}


void clt_initChannel(void) {

    is_server=false;

    socket_client = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_client == -1)
    {
        printf("Could not create socket\n");
    }

    server.sin_addr.s_addr = inet_addr(server_ip);
    server.sin_family = AF_INET;
    server.sin_port = htons(port);

}

int srv_sendData(Connection * conection, Datagram * datagram) {

    int arguments_size = sizeof(Data);

    void * package = calloc(arguments_size, 1); 
    memcpy(package, datagram->arguments, arguments_size);

    write(request_sock, package, arguments_size);
    close(request_sock);

    free(package);

    return 0;
}

int clt_sendData(Connection * connection, Datagram * datagram) {

    int arguments_size = sizeof(Data);

    void * package = calloc(arguments_size, 1);
    memcpy(package, datagram->arguments, arguments_size);

    if ((socket_client = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf ("Couldn't create socket\n");
    }

    if ((connect(socket_client, (struct sockaddr *)&server, sizeof(server))) >= 0) {
        if (send(socket_client, package, *((int * )package), MSG_NOSIGNAL) < 0) {
            printf("Send failed\n");
            return -1;
        }
    }
    else
        return -1;

    free(package);
    
    return 0;
}

void srv_receiveData(Connection * connection, Datagram * datagram) {

    //get socket fd that is trying to connect with server
    if ((request_sock = accept(socket_server, (struct sockaddr *)&client, (socklen_t*)&clistruct_size))) {

        void * package = calloc(MAX_RDATA_SIZE, 1);

        int read_size;

        //recieve information from fd
        if ((read_size = recv(request_sock, package, MAX_RDATA_SIZE, 0)) > 0) {
            datagram->arguments = calloc(read_size, 1); 
            memcpy(datagram->arguments, package, read_size);
        }

        if (request_sock < 0) {
            printf ("Port is being used.\n");
            return;
        }
        
        free(package);
    }
}

void clt_receiveData(Connection * connection, Datagram * datagram) {

    void * package = calloc(MAX_RDATA_SIZE, 1);
    
    int read_size;

    if ((read_size = recv(socket_client, package, MAX_RDATA_SIZE, 0)) < 0) {
        puts("Server is not avaiable.\n");
        return;
    }

    close(socket_client);

    datagram->arguments = calloc(read_size, 1); 
    memcpy(datagram->arguments, package, read_size );

    free(package);
}

