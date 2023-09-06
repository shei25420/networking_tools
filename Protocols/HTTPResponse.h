//
// Created by z3r0 on 8/23/23.
//
#ifndef SCRAPER_HTTPRESPONSE_H
#define SCRAPER_HTTPRESPONSE_H

#include "../DataStructures/Trees/BinaryTree.h"

#include "HttpClient.h"
typedef struct http_response_header {
    char* key;
    char* value;
} HTTPResponseHeader;

typedef enum content_type {
    Application,
    Audio,
    Image,
    MultiPart,
    Text,
    Video,
    VND
} ContentType;

typedef struct http_response {
    BinaryTree * response_headers;
    long statusCode;
    char statusText[100];
    char httpVersion[20];
    ContentType contentType;
    void* body;
    long bodyLen;
    int retry;
    int (*recv_response)(HTTPClient *client, struct http_response* http_response);
} HTTPResponse;

HTTPResponse * http_response_constructor();
void http_response_deconstructor(HTTPResponse* response);
#endif //SCRAPER_HTTPRESPONSE_H
