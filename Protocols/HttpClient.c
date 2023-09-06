//
// Created by z3r0 on 8/23/23.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "HttpClient.h"

int handle_ssl_conn_fail (HTTPClient* client);
int http_close_connection(HTTPClient* client);
int http_make_connection (HTTPClient* client);
int reconnect (HTTPClient *client);

HTTPClient * http_client_constructor(char* host, char* port) {
    HTTPClient * client = (HTTPClient*)calloc(1, sizeof(HTTPClient));
    if (!client) {
        fprintf(stderr, "[-] error allocating memory for http client");
        return NULL;
    }

    client->host = (char *)calloc(1, strlen(host) + 1);
    if (!client->host) {
        perror("Error allocating memory for client host");
        return 0;
    }
    strncpy(client->host, host, strlen(host));

    client->port = (char *)calloc(1, strlen(port) + 1);
    if (!client->port) {
        perror("Error allocating memory for client port");
        free(client->host);
        return 0;
    }
    strncpy(client->port, port, strlen(port));

    client->connect = http_make_connection;
    client->reconnect = reconnect;
    return client;
}

void http_client_deconstructor(HTTPClient* client) {
    free(client->host);
    free(client->port);
    handle_ssl_conn_fail(client);
    free(client);
}

int http_make_connection (HTTPClient* client) {
    SSL_METHOD *method;
    OpenSSL_add_all_algorithms();

    client->ctx = SSL_CTX_new(SSLv23_client_method());
    if (!client->ctx) {
        ERR_print_errors_fp(stderr);
        return 0;
    }

    if (!(client->sBio = BIO_new_ssl_connect(client->ctx))) {
        perror("Error creating new ssl connection");
        handle_ssl_conn_fail(client);
        return 0;
    }

    BIO_get_ssl(client->sBio, &client->ssl);
    if (!client->ssl) {
        perror("Error getting ssl handle");
        handle_ssl_conn_fail(client);
        return 0;
    }

    BIO_set_conn_hostname(client->sBio, client->host);
    BIO_set_conn_port(client->sBio, client->port);

    if (!(SSL_set_tlsext_host_name(client->ssl, client->host))) {
        perror("Error setting tls ext hostname");
        handle_ssl_conn_fail(client);
        return 0;
    }

    if (BIO_do_connect(client->sBio) != 1) {
        fprintf(stderr, "Error connecting to server\n");
        ERR_print_errors_fp(stderr);
        handle_ssl_conn_fail(client);
        return 0;
    }

    printf("Connection to %s:%s successful\n", client->host, client->port);
    return 1;
}


int handle_ssl_conn_fail (HTTPClient* client) {
    if (!(client->sBio)) BIO_free_all(client->sBio);
    if (!(client->ctx)) SSL_CTX_free(client->ctx);
    return 0;
}

int reconnect (HTTPClient *client) {
    handle_ssl_conn_fail(client);
    http_make_connection(client);
    return 1;
}