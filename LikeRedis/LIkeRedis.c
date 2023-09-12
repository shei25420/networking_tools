//
// Created by shei on 9/12/23.
//

#include "../Protocols/TCP/TCPServer.h"
#include "LIkeRedis.h"

void handle_connection (int sockFd) {

}

int test_redis () {
    TCPServer * server = tcp_server_constructor(false, false, 10);
    if (!server) return -1;


    tcp_server_deconstructor(server);
}