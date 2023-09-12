//
// Created by shei on 9/12/23.
//

#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include "TCPClient.h"

ssize_t send_data(void* data, int dataLen, struct tcp_client* client);
ssize_t recv_data(void* buffer, struct tcp_client* client);

bool connect_server (char * hostname, char* port, TCPClient* client);

TCPClient *tcp_client_constructor() {
    int sockFd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockFd < 0) {
        perror("[-] error initializing socket");
        return NULL;
    }
    printf("[+] socket initialization successful\n");

    TCPClient *client = (TCPClient*)calloc(1, sizeof(TCPClient));
    if (!client) {
        perror("[-] error allocating memory for tcp client ");
        return NULL;
    }

    client->cliFd = sockFd;
    client->recv_data = recv_data;
    client->send_data = send_data;
    client->connect = connect_server;

    printf("[+] TCP client initialized successfully\n");
    return client;
}

void tcp_client_deconstructor(TCPClient* client) {
    close(client->cliFd);
    free(client);
}

bool connect_server (char * hostname, char* port, TCPClient* client) {
    printf("[+] Attempting to Connect to %s port %s\n", hostname, port);
    struct addrinfo hints = { 0 };
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = 0;

    struct addrinfo* serverAddr = 0;
    int ret = getaddrinfo(hostname, port, &hints, &serverAddr);
    if (ret) {
        printf("[-] error resolving hostname: %s\n", gai_strerror(ret));
        return false;
    }

    if (connect(client->cliFd, serverAddr->ai_addr, serverAddr->ai_addrlen)) {
        perror("[-] error connecting to host");
        freeaddrinfo(serverAddr);
        return false;
    }

    freeaddrinfo(serverAddr);
    printf("[+] connection to %s successful\n", hostname);
    return true;
}

ssize_t send_data(void* data, int dataLen, struct tcp_client* client) {
    printf("Sending %d bytes of data\n", dataLen);

    ssize_t totalSent = 0;
    void* cursor = data;
    while (totalSent != dataLen) {
        ssize_t bytesSent = send(client->cliFd, (((unsigned char*)data) + totalSent), (totalSent - dataLen), 0);
        if (bytesSent < 0) {
            perror("[-] error sending data");
            return -1;
        }
        totalSent += bytesSent;
    }
    return totalSent;
}

ssize_t recv_data(void* buffer, struct tcp_client* client) {

}