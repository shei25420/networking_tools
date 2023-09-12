//
// Created by z3r0 on 8/23/23.
//

#ifndef SCRAPER_HTTPREQUEST_H
#define SCRAPER_HTTPREQUEST_H

#include "../HttpClient.h"

typedef enum http_request_type {
    GET,
    POST,
    PATCH,
    DELETE
} HTTPRequestType;

typedef struct http_request {
    char* req_path;
    int (*send_request)(HTTPRequestType req_type, char** headers, char* data, HTTPClient* client, struct http_request* request);
} HTTPRequest;

HTTPRequest * http_request_constructor (char* req_path);
void http_request_deconstructor(HTTPRequest *request);

#endif //SCRAPER_HTTPREQUEST_H
