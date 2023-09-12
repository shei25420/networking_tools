//
// Created by shei on 9/12/23.
//

#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include "TCPServer.h"

bool start_server(int port, handle_client_f clientFunc,TCPServer *server);

TCPServer * tcp_server_constructor (bool secure, bool blocking, int maxConn) {
    int sockFd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockFd < 0) {
        perror("[-] error creating socket");
        return NULL;
    }
    printf("[+] socket initialization success\n");

    TCPServer *server = (TCPServer*)calloc(1, sizeof(TCPServer));
    if (!server) {
        perror("[-] error allocating memory for server struct");
        close(sockFd);
        return NULL;
    }

    server->maxConn = maxConn ? maxConn : 10;
    server->sockFd = sockFd;
    server->secure = secure;
    server->blocking = blocking;

    server->start_server = start_server;
    return server;
}

void tcp_server_deconstructor(TCPServer* server) {
    close(server->sockFd);
    free(server);
}

bool start_server(int port, handle_client_f clientFunc,  TCPServer* server) {
    //Bind To Address
    struct sockaddr_in serv_addr = { 0 };
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = ntohs(port);
    serv_addr.sin_addr.s_addr = ntohl(INADDR_LOOPBACK);

    if (bind(server->sockFd, (struct sockaddr*)&serv_addr, sizeof(serv_addr))) {
        perror("[-] error binding to server");
        return false;
    }

    printf("[+] binding to interface complete");

    //Start Listening For Connections
    if (listen(server->sockFd, server->maxConn)) {
        perror("[-] error listening for connections");
        return false;
    }

    //Implement SSL if secure server is required

    //Start Accepting Connections
    struct sockaddr_in cliAddr;
    socklen_t cliAddrLen = sizeof(cliAddr);
    while (true) {
        memset(&cliAddr, 0, sizeof(cliAddr));

        int clientSock = accept(server->sockFd, (struct sockaddr*)&cliAddr, &cliAddrLen);
        if (clientSock < 0) {
            perror("[-] error accepting remote client");
            continue;
        }
        clientFunc(clientSock);
    }

    return true;
}