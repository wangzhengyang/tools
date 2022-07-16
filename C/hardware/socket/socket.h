#ifndef SOCKET_H
#define SOCKET_H

#include <stdint.h>

typedef enum
{
    SOCKET_CONFIGPARAM_KEEPALIVE,
    SOCKET_CONFIGPARAM_KEEPIDLE,
    SOCKET_CONFIGPARAM_KEEPALIVE_INTERVAL,
    SOCKET_CONFIGPARAM_KEEPALIVE_CNT,
    SOCKET_COFNIGPARAM_USER_TIMEOUT,
    SOCKET_CONFIGPARAM_REUSEADDR,
    SOCKET_CONFIGPARAM_NODELAY,
    SOCKET_CONFIGPARAM_USER_TIMEOUT,
}socket_configparam_e;

typedef struct tcpserver_s* tcpserver_t;
typedef struct tcpsocket_s* tcpsocket_t;
typedef struct udpsocket_s* udpsocket_t;

int socket_set_configparam(int socketfd, socket_configparam_e param, int value);

int socket_set_nonblocking(int socketfd);

tcpserver_t tcpserver_create(char* address, int port);

int tcpserver_listen(tcpserver_t socket);

tcpsocket_t tcpserver_accept(tcpserver_t socket);

void tcpserver_set_backlog(tcpserver_t socket, int backlog);

void tcpserver_destroy(tcpserver_t socket);

tcpsocket_t tcpsocket_create();

int tcpsocket_connect_async(tcpsocket_t self, const char* address, int port);

int tcpsocket_check_connectstate(tcpsocket_t self);

int tcpsocket_connect(tcpsocket_t socket, char* addr, int port);

char* tcpsocket_get_peeraddress(tcpsocket_t self);

char* tcpsocket_get_localaddress(tcpsocket_t self);

int tcpsocket_read(tcpsocket_t self, uint8_t* buf, int size);

int tcpsocket_write(tcpsocket_t self, uint8_t* buf, int size);

void tcpsocket_destroy(tcpsocket_t self);




#endif