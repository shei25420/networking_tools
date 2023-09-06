//
// Created by z3r0 on 8/23/23.
//

#ifndef SCRAPER_HTTPCLIENT_H
#define SCRAPER_HTTPCLIENT_H
#include <openssl/ssl.h>
#include <openssl/err.h>

typedef struct http_client {
    char* host;
    char* port;

    BIO* sBio;
    SSL* ssl;
    SSL_CTX * ctx;

    int (*connect)(struct http_client* client);
    int (*reconnect)(struct http_client* client);
} HTTPClient;

HTTPClient * http_client_constructor(char* host, char* port);
void http_client_deconstructor(HTTPClient* client);

#endif //SCRAPER_HTTPCLIENT_H
