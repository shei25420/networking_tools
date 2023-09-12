//
// Created by z3r0 on 8/23/23.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "HTTPRequest.h"

int http_send_request (HTTPRequestType req_type, char** headers, char* data, HTTPClient* client, HTTPRequest* request);

HTTPRequest * http_request_constructor (char* req_path) {
    HTTPRequest* request = (HTTPRequest*)calloc(1, sizeof(HTTPRequest));
    if (!request) {
        perror("Error allocating memory for http request");
        return NULL;
    }
    request->req_path = (char *)calloc(1, strlen(req_path) + 1);
    if (!request->req_path) {
        perror("Error allocating request path memory");
        free(request);
        return NULL;
    }
    strncpy(request->req_path, req_path, strlen(req_path));

    request->send_request = http_send_request;
    return request;
}

void http_request_deconstructor(HTTPRequest *request) {
    free(request->req_path);
    free(request);
}

char* generate_request_framework (HTTPRequestType req_type, char* req_path, char* host, char** headers, char* data) {
    char type[20] = { 0 }, custom_headers[3096] = { 0 };
    switch (req_type) {
        case GET:
            strcpy(type, "GET");
            break;
        case POST:
            strcpy(type, "POST");
            break;
        case PATCH:
            strcpy(type, "PATCH");
            break;
        case DELETE:
            strcpy(type, "DELETE");
            break;
    }

    char* req_framework =
            "%s %s HTTP/1.1\r\n"
            "Connection: keep-alive\r\n"
            "Host: %s\r\n"
            "%s\r\n\r\n";
//            (req_type != GET ? "%s": '\0');

    if (headers && headers[0] != NULL) {
        for (int i = 0; i < (sizeof(headers) / sizeof(headers[0])); i++) {
            strncpy(custom_headers, headers[i], (strlen(custom_headers) - sizeof(custom_headers)));
        }
    }

    size_t req_len = strlen(req_framework) + strlen(type) + strlen(req_path) + strlen(host) + strlen(custom_headers);
    char* request = (char *)calloc(1, req_len + 1);
    if (!request) {
        perror("Error allocating request memory");
        return NULL;
    }

    snprintf(request, req_len, req_framework, type, req_path, host, custom_headers);
    return request;
}

int http_send_request (HTTPRequestType req_type, char** headers, char* data, HTTPClient* client, HTTPRequest* request) {
    int status = 0;
    char* q = 0, *req_path_pointer = request->req_path;
    if ((q = strstr(req_path_pointer, "../"))) {
        req_path_pointer = q + 2;
    } else if (req_path_pointer[0] == '.' && req_path_pointer[1] == '/') {
        req_path_pointer ++;
    }

    char* req = generate_request_framework(req_type, req_path_pointer, client->host, headers, data);
    if (!req) return status;

    size_t  req_len = strlen(req);
    int bytes_sent = BIO_write(client->sBio, req, (int)req_len);
    if (bytes_sent < 0 ) {
        ERR_print_errors_fp(stderr);
        goto final;
    }

    status = 1;
    final:
        free(req);
        return status;
}