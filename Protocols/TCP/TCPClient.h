//
// Created by shei on 9/12/23.
//

#ifndef SCRAPER_TCPCLIENT_H
#define SCRAPER_TCPCLIENT_H

#include <stdbool.h>

typedef struct tcp_client {
    int cliFd;
    bool (*connect)(char* hostname, char* port, struct tcp_client* client);

    ssize_t (*send_data)(void* data, int dataLen, struct tcp_client* client);
    ssize_t (*recv_data)(void* buffer, struct tcp_client* client);
} TCPClient;

TCPClient *tcp_client_constructor();
void tcp_client_deconstructor(TCPClient* client);

#endif //SCRAPER_TCPCLIEtruNT_H
