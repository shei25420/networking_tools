//
// Created by shei on 9/12/23.
//

#ifndef SCRAPER_TCPSERVER_H
#define SCRAPER_TCPSERVER_H
#include <stdbool.h>

typedef void (*handle_client_f)(int clientConn);

typedef struct tcp_server {
    int maxConn;
    bool secure;
    bool blocking;

    int sockFd;
    bool (*start_server)(int port, handle_client_f, struct tcp_server* server);
} TCPServer;

TCPServer * tcp_server_constructor (bool secure, bool blocking, int maxConn);
void tcp_server_deconstructor(TCPServer* server);

#endif //SCRAPER_TCPSERVER_H
