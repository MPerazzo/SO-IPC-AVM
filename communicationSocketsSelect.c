#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include "types.h"
#include "comms.h"

#define port 4000
#define server_ip "127.0.0.1"
#define MAXQSIZE 5
#define MAX_RDATA_SIZE 100



struct sockaddr_in server, client;

int socket_server;

int clistruct_size;

bool is_server;

Connection * srv_initChannel();
Connection * clt_initChannel();
int srv_sendData(Connection *, Datagram *);
int clt_sendData(Connection *, Datagram *);
int srv_receiveData(Connection *, Datagram *);
int clt_receiveData(Connection *, Datagram *);


Connection * initChannel(int bool_server) {

    switch (bool_server) {

    case true:
        return srv_initChannel();
        break;

    case false:
        return clt_initChannel();
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

int receiveData(Connection * connection, Datagram * params) {
    switch (is_server) {
    case true:
        return srv_receiveData(connection, params);
        break;

    case false:
        return clt_receiveData(connection, params);
        break;
    }
}

Connection * srv_initChannel(void) {

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
        exit(1);
    }

    //Listen for connections from socket
    listen(socket_server, MAXQSIZE);

    clistruct_size = sizeof(struct sockaddr_in);

    return createConnection(socket_server);

}


Connection * clt_initChannel(void) {

	int socket_client;

    is_server=false;

    if (socket_client = socket(AF_INET, SOCK_STREAM, 0)<0)
    	printf ("Couldn't create socket\n");

    server.sin_addr.s_addr = inet_addr(server_ip);
    server.sin_family = AF_INET;
    server.sin_port = htons(port);

    return createConnection(socket_client);
}

int srv_sendData(Connection * connection, Datagram * datagram) {

    int arguments_size = sizeof(Data);

    void * package = calloc(arguments_size, 1); 
    memcpy(package, datagram->arguments, arguments_size);

    write(connection->id, package, arguments_size);
    disConnect(connection);

    free(package);

    return 0;
}

int clt_sendData(Connection * connection, Datagram * datagram) {

    int arguments_size = sizeof(Data);

    void * package = calloc(arguments_size, 1);
    memcpy(package, datagram->arguments, arguments_size);

    if ((connection->id = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf ("Couldn't create socket\n");
        return -1;
    }

    if ((connect(connection->id, (struct sockaddr *)&server, sizeof(server)) >= 0)) {
        if (send(connection->id, package, *((int * )package), MSG_NOSIGNAL) < 0) {
            printf("Send failed\n");
            return -1;
        }
    }
    else {
    	printf("Couldn´t connect to server\n");
        return -1;
    }

    free(package);
    
    return 0;
}

int srv_receiveData(Connection * connection, Datagram * datagram) {
        
    void * package = calloc(MAX_RDATA_SIZE, 1);

    int read_size;

    //recieve information from fd
    if ((read_size = recv(connection->id, package, MAX_RDATA_SIZE, 0)) < 0) {
    	printf("Could receive information from socket\n");
    	return -1;
    }
    
    datagram->arguments = calloc(read_size, 1); 
    memcpy(datagram->arguments, package, read_size);
       
    free(package);
    
    return read_size;
}

int clt_receiveData(Connection * connection, Datagram * datagram) {

    void * package = calloc(MAX_RDATA_SIZE, 1);
    
    int read_size;

    if ((read_size = recv(connection->id, package, MAX_RDATA_SIZE, 0)) < 0) {
        puts("Server is not avaiable.\n");
        return -1;
    }

    disConnect(connection);

    datagram->arguments = calloc(read_size, 1); 
    memcpy(datagram->arguments, package, read_size );

    free(package);

    return read_size;
}

 int acceptConnection(Connection * connection) {
    
    int socket_client;

    //get socket fd that is trying to connect with server
    if ((socket_client = accept(socket_server, (struct sockaddr *)&client, (socklen_t*)&clistruct_size))<0) {
            printf ("Port is being used.\n");
            exit(1);
    }
    else {
    	connection->id = socket_client;
        return socket_client;
    }
}

Connection * createConnection(int id){
  
  Connection * connection;
  connection = malloc(sizeof(Connection));
  connection -> id = id;
  return connection;
}

void disConnect(Connection * connection) {
    close(connection->id);
}
