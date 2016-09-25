#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>

#include "types.h"
#include "comm.h"

struct Connection {

    int socket_descriptor;

};

struct Listener {

    int listener_fd;

};

Connection * comm_connect(char * address) {

    int socket_fd, len;
    struct sockaddr_un remote;
    Connection * connection;

    connection = malloc(sizeof(Connection));

    remote.sun_family = AF_UNIX;
    strcpy(remote.sun_path, address);
    len = strlen(remote.sun_path) + sizeof(remote.sun_family);

    socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);

    connect(socket_fd, (struct sockaddr *)&remote, len);

    connection->socket_descriptor = socket_fd;

    return connection;

}

Listener * comm_listen(char * address) {

    int socket_fd, len;
    struct sockaddr_un local;
    Listener * listener;

    listener = malloc(sizeof(Listener));

    local.sun_family = AF_UNIX;
    strcpy(local.sun_path, address);
    len = strlen(local.sun_path) + sizeof(local.sun_family);

    listener->listener_fd = socket(AF_UNIX, SOCK_STREAM, 0);

    unlink(local.sun_path);

    bind(listener->listener_fd, (struct sockaddr *)&local, len);

    listen(listener->listener_fd, 5);

    return listener;

}

Connection * comm_accept(Listener * listener) {

    int socket_fd, len;
    struct sockaddr_un remote;
    Connection * connection;

    connection = malloc(sizeof(Connection));

    len = sizeof(struct sockaddr_un);

    socket_fd = accept(listener->listener_fd, &remote, &len);

    connection->socket_descriptor = socket_fd;

    return connection;

}

int comm_write(Connection * connection, char * dataToWrite, int size) {

    return write(connection->socket_descriptor, dataToWrite, size);

}

int comm_read(Connection * connection, char * dataToRead, int size) {

    return read(connection->socket_descriptor, dataToRead, size);

}

int comm_disconnect(Connection * connection) {

    return close(connection->socket_descriptor);

}
