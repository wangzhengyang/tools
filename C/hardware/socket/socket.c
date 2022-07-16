#include "socket.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <errno.h>
#include <sys/select.h>
#include <time.h>
#include <fcntl.h>

#ifndef SOL_TCP
#define SOL_TCP IPPROTO_TCP
#endif

struct tcpsocket_s
{
    int fd;
    uint32_t connectTimeout;
};

struct udpsocket_s
{
    int fd;
};

struct tcpserver_s
{
    int fd;
    int backlog;
};

int socket_set_configparam(int socketfd, socket_configparam_e param, int value)
{
    int optval;
    int ret = -1;

    switch (param)
    {
    case SOCKET_CONFIGPARAM_KEEPALIVE:
        optval = 1;
        ret = setsockopt(socketfd, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval));
        break;
    case SOCKET_CONFIGPARAM_KEEPIDLE:
        optval = value;
        ret = setsockopt(socketfd, SOL_TCP, TCP_KEEPIDLE, &optval, sizeof(optval));
        break;
    case SOCKET_CONFIGPARAM_KEEPALIVE_INTERVAL:
        optval = value;
        ret = setsockopt(socketfd, SOL_TCP, TCP_KEEPINTVL, &optval, sizeof(optval));
        break;
    case SOCKET_CONFIGPARAM_KEEPALIVE_CNT:
        optval = value;
        ret = setsockopt(socketfd, SOL_TCP, TCP_KEEPCNT, &optval, sizeof(optval));
        break;
    case SOCKET_CONFIGPARAM_REUSEADDR:
        optval = 1;
        ret = setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
        break;
    case SOCKET_CONFIGPARAM_USER_TIMEOUT:
        optval = value;
        ret = setsockopt(socketfd, SOL_TCP, TCP_USER_TIMEOUT, &optval, sizeof(optval));
        break;
    case SOCKET_CONFIGPARAM_NODELAY:
        optval = 1;
        ret = setsockopt(socketfd, IPPROTO_IP, TCP_NODELAY, &optval, sizeof(optval));
        break;
    }
    return ret;
}

int socket_set_nonblocking(int socketfd)
{
    int flags = fcntl(socketfd, F_GETFL, 0);

    return fcntl(socketfd, F_SETFL, flags | O_NONBLOCK);
}

tcpserver_t tcpserver_create(char* address, int port)
{
    tcpserver_t server;

    int fd = socket(AF_INET, SOCK_STREAM, 0);

    if (fd < 0)
    {
        return NULL;
    }

    struct sockaddr_in serveraddr;
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(port);


    if (bind(fd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) < 0)
    {
        close(fd);
        return NULL;
    }

    server->fd = fd;
    server->backlog = 0;
    return server;
}

int tcpserver_listen(tcpserver_t socket)
{
    return listen(socket->fd, socket->backlog);
}

tcpsocket_t tcpserver_accept(tcpserver_t socket)
{
    int fd;
    tcpsocket_t consocket;

    fd = accept(socket->fd, NULL, NULL);

    if (fd < 0)
    {
        return NULL;
    }

    consocket = (tcpsocket_t)malloc(sizeof(struct tcpsocket_s));

    if (!consocket)
    {
        close(fd);
        return NULL;
    }

    consocket->fd = fd;

    return consocket;
}

void tcpserver_set_backlog(tcpserver_t socket, int backlog)
{
    socket->backlog = backlog;
}

static void shutdown_close_socket(int fd)
{
    if (fd != -1)
    {
        int res = shutdown(fd, SHUT_RDWR);
        close(fd);
    }
}

void tcpserver_destroy(tcpserver_t socket)
{
    shutdown_close_socket(socket->fd);
    free(socket);
}

tcpsocket_t tcpsocket_create()
{
    tcpsocket_t self;

    int fd = socket(AF_INET, SOCK_STREAM, 0);

    if (fd < 0)
    {
        return NULL;
    }

    self = (tcpsocket_t)malloc(sizeof(struct tcpsocket_s));

    if (!self)
    {
        close(fd);
        return NULL;
    }

    self->fd = fd;
    self->connectTimeout = 5000;

    return socket;
}

int tcpsocket_connect_async(tcpsocket_t self, const char* address, int port)
{
    struct sockaddr_in serveraddr;

    memset(&serveraddr, 0, sizeof(serveraddr));

    serveraddr.sin_family = AF_INET;

    inet_pton(AF_INET, &serveraddr.sin_addr.s_addr, address);

    serveraddr.sin_port = htons(port);

    if (connect(self->fd, (struct socketaddr*)&serveraddr, sizeof(serveraddr)) < 0)
    {
        if (errno != EINPROGRESS)
        {
            return -1;
        }
    }

    return 0;
}

int tcpsocket_check_connectstate(tcpsocket_t self)
{
    struct timeval timeout;
    int ret;

    timeout.tv_sec = 0;
    timeout.tv_usec = self->connectTimeout;

    fd_set fdset;
    FD_ZERO(&fdset);
    FD_SET(self->fd, &fdset);

    while (1)
    {
        ret = select(self->fd + 1, NULL, &fdset, NULL, &timeout);
        if (ret == 1)
        {
            int so_error;
            socklen_t len = sizeof(int);
            ret = getsockopt(self->fd, SOL_SOCKET, SO_ERROR, &so_error, &len);
            if ((ret < 0) || (so_error != 0))
            {
                return -1;
            }
            return 0;
        }
        else if (ret < 0)
        {
            return -1;
        }
    }
    return -1;
}

int tcpsocket_connect(tcpsocket_t self, char* addr, int port)
{
    struct sockaddr_in serveraddr;

    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    inet_pton(AF_INET, &serveraddr.sin_addr.s_addr, addr);
    serveraddr.sin_port = htons(port);

    self->fd = socket(AF_INET, SOCK_STREAM, 0);
    if (connect(self->fd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) < 0)
    {
        return -1;
    }
    return 0;
}

static char* convert_address_to_str(struct sockaddr_in* serveraddr)
{
    char* string;
    char* addr_string[INET_ADDRSTRLEN];
    int port;

    if (serveraddr->sin_family != AF_INET)
    {
        return NULL;
    }

    port = ntohs(serveraddr->sin_port);
    inet_ntop(AF_INET, serveraddr->sin_addr.s_addr, addr_string, sizeof(addr_string));
    string = malloc(sizeof(INET_ADDRSTRLEN + 8));
    snprintf(string, INET_ADDRSTRLEN + 8, "%s[%i]", addr_string, port);
    return string;
}

char* tcpsocket_get_peeraddress(tcpsocket_t self)
{
    struct sockaddr_in serveraddr;
    socklen_t socket_len = sizeof(serveraddr);
    char* string;

    getpeername(self->fd, &serveraddr, &socket_len);

    return convert_address_to_str(&serveraddr);
}

char* tcpsocket_get_localaddress(tcpsocket_t self)
{
    struct sockaddr_in serveraddr;
    socklen_t socket_len = sizeof(serveraddr);
    char* string;

    getsockname(self->fd, &serveraddr, &socket_len);

    return convert_address_to_str(&serveraddr);
}

int tcpsocket_read(tcpsocket_t self, uint8_t* buf, int size)
{
    int ret = recv(self->fd, buf, size, MSG_DONTWAIT);

    if (ret == 0)
    {
        return -1;
    }

    if (ret < 0)
    {
        if (errno == EAGAIN)
        {
            return 0;
        }

        return -1;
    }

    return ret;
}

int tcpsocket_write(tcpsocket_t socket, uint8_t* buf, int size)
{
    //MSG_NOSIGNAL - prevent send to signal SIGPIPE when peer unexpectedly closed the socket
    int ret = send(socket->fd, buf, size, MSG_NOSIGNAL | MSG_DONTWAIT);

    if (ret < 0)
    {
        if (errno == EAGAIN)
        {
            return 0;
        }
    }

    return -1;
}

void tcpsocket_destroy(tcpsocket_t socket)
{
    shutdown_close_socket(socket->fd);
    free(socket);
}


