//
// Created by z3r0 on 8/23/23.
//
#define _GNU_SOURCE
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <openssl/bio.h>
#include "HTTPResponse.h"

typedef enum response_encoding {
    ContentLength,
    Chunked,
    Gzip,
    Deflate
} ResponseEncoding;

int http_recv_response (HTTPClient* client, struct http_response* http_response);
int http_header_cmp (void* data1, void* data2);
int check_response_resize (void** response, int *responseSize, unsigned char** current_resp_pointer, unsigned char** end_resp_pointer);
int parse_response (void* response, HTTPResponse * http_response);
int parse_headers (char* response_headers, char** response_headers_end, HTTPResponse* http_response);

HTTPResponse * http_response_constructor() {
    HTTPResponse * response = (HTTPResponse*)calloc(1, sizeof(HTTPResponse));
    if (!response) return NULL;

    response->response_headers = binary_tree_constructor(http_header_cmp);
    if (!response->response_headers) {
        free(response);
        return NULL;
    }

    response->recv_response = http_recv_response;
    return response;
}

void http_response_deconstructor(HTTPResponse* response) {
    if (response->body) free(response->body);

    binary_tree_deconstructor(response->response_headers);
    free(response);
}

int http_header_cmp (void* data1, void* data2) {
    HTTPResponseHeader* header1 = (HTTPResponseHeader*)data1;
    HTTPResponseHeader* header2 = (HTTPResponseHeader*)data2;

    int cmp = strcasecmp(header1->key, header2->key);
    if (cmp == 0) return 0;
    else if (cmp < 0) return -1;
    return 1;
}

int http_recv_response (HTTPClient* client, struct http_response* http_response) {
    int response_size = 1024, status = 0;
    long headersSize = 0;
    unsigned char* current_resp_pointer, *end_resp_pointer;
    char* q = 0, *resp_headers_end_pointer = 0;
    void* response = calloc(1, response_size);
    if (!response) {
        perror("Error allocating response buffer");
        return status;
    }

    current_resp_pointer = response;
    end_resp_pointer = (unsigned char*)response + response_size;
    size_t totalRead = 0;

    while (1) {
        size_t bytes_read = 0;
        if ((BIO_read_ex(client->sBio, current_resp_pointer, (int)(end_resp_pointer - current_resp_pointer), &bytes_read) == 0)) {
            ERR_print_errors_fp(stderr);
            break;
        }

        totalRead += bytes_read;
        if (bytes_read == 0) {
            perror( "[debug] finished reading???");
            break;
        } else if (bytes_read < 0) {

            break;
        }

        current_resp_pointer += bytes_read;
        if (!http_response->bodyLen && (resp_headers_end_pointer = strstr(response, "\r\n\r\n"))) {
            headersSize = ((resp_headers_end_pointer - (char *)response) + 4);
            parse_headers(response, &resp_headers_end_pointer, http_response);

            //CHeck for content Length Header
            HTTPResponseHeader searchHeader = {0};
            searchHeader.key = "content-length";

            HTTPResponseHeader * contentLengthHeader = http_response->response_headers->search(&searchHeader, http_response->response_headers);
            if (!contentLengthHeader) {
                fprintf(stderr, "[-] dont know what to do with this encoding shit yet\n");
                return 0;
            } else {
                http_response->bodyLen = strtol(contentLengthHeader->value, NULL, 10);
            }
        }

        if (!(check_response_resize(&response, &response_size, &current_resp_pointer, &end_resp_pointer))) {
            break;
        }
    }
    if (totalRead == 0 || (http_response->bodyLen != (totalRead - headersSize))) {
        http_response->retry = 1;
    } else {
        parse_response(response, http_response);
    }

    status = 1;
    free(response);
    return status;
}

int check_response_resize (void** response, int *responseSize, unsigned char** current_resp_pointer, unsigned char** end_resp_pointer) {
    if (*current_resp_pointer == *end_resp_pointer) {
        long long resp_pointer_offset = ((*current_resp_pointer) - ((unsigned char*)*response));
        (*responseSize) *= 2;

        void* new_response = realloc(*response, (*responseSize));
        if (!new_response) {
            perror("Error reallocating memory");
            return 0;
        }

        *response = new_response;
        *current_resp_pointer = (unsigned char*)(*response) + resp_pointer_offset;
        *end_resp_pointer = (unsigned char*)(*response) + (*responseSize);
    }
    return 1;
}

int parse_headers (char* response_headers, char** response_headers_end, HTTPResponse* http_response) {
    char* header_end = 0, *q = 0, *key_end = 0, *val_start = 0, *resp_current_pointer = response_headers;
    long keySize = 0, valSize = 0;

    //Parse Response line
    char* res_header_line_end = strstr((char *)response_headers, "\r\n");
    if (!res_header_line_end) {
        fprintf(stderr, "[-] response seems invalid");
        return 0;
    }

    while (resp_current_pointer != res_header_line_end) {
        if (!(q = strstr(resp_current_pointer, " "))) {
            fprintf(stderr, "[-] response seems invalid");
            return 0;
        }

        if (!(strlen(http_response->httpVersion))) {
            strncpy(http_response->httpVersion, resp_current_pointer, (q - resp_current_pointer));
        } else if (!http_response->statusCode) {
            http_response->statusCode = strtol(resp_current_pointer, &q, 10);
        } else if (!(strlen(http_response->statusText))) {
            strncpy(http_response->statusText, resp_current_pointer, (res_header_line_end - resp_current_pointer));
            resp_current_pointer = res_header_line_end + 2;
            break;
        }

        resp_current_pointer = q + 1;
    }

    //Parse Other response headers
    while (resp_current_pointer < (*response_headers_end)) {
        if (!(header_end = strstr(resp_current_pointer, "\r\n"))) {
            fprintf(stderr, "[-] response headers seems malformed\n");
            return 0;
        }

        *header_end = 0;
        key_end = strchr(resp_current_pointer, ':');
        if (!key_end) {
            fprintf(stderr, "[-] response header seems malformed: %s", response_headers);
            return 0;
        }

        keySize = (key_end - resp_current_pointer);

        val_start = key_end + 1;
        valSize = header_end - val_start;

        HTTPResponseHeader responseHeader = { 0 };
        responseHeader.key = (char *)calloc(1, (keySize + 1));
        if (!responseHeader.key) {
            perror("Error allocating response header key memory");
            return 0;
        }
        strncpy(responseHeader.key, resp_current_pointer, keySize);

        responseHeader.value = (char *)calloc(1, (valSize + 1));
        if (!responseHeader.value) {
            perror("Error allocating response header value memory");
            return 0;
        }
        strncpy(responseHeader.value, val_start, valSize);

        http_response->response_headers->insert(&responseHeader, sizeof(responseHeader), http_response->response_headers);
        *header_end = '\r';
        resp_current_pointer = header_end + 2;
    }

    return 1;
}

int parse_response (void* response, HTTPResponse * http_response) {
    char* resp_current_pointer = (char *)response, *q = 0;

    char* resp_header_end_pointer = strstr(resp_current_pointer, "\r\n\r\n");
    if (!resp_header_end_pointer) {
        fprintf(stderr, "[-] could not parse response header. looks like an invalid response");
        return 0;
    }


    //Content Type Response
    HTTPResponseHeader searchHeader = {0};
    searchHeader.key = "Content-Type";

    HTTPResponseHeader *contentTypeHeader = http_response->response_headers->search(&searchHeader, http_response->response_headers);
    if (!contentTypeHeader) {
        fprintf(stderr, "[-] could not find content type header\n");
        return 0;
    }

    if(strcasestr(contentTypeHeader->value, "text")) {
        http_response->contentType = Text;
    } else if (strcasestr(contentTypeHeader->value, "audio")) {
        http_response->contentType = Audio;
    } else if (strcasestr(contentTypeHeader->value, "image")) {
        http_response->contentType = Image;
    } else if (strcasestr(contentTypeHeader->value, "multipart")) {
        http_response->contentType = MultiPart;
    } else if (strcasestr(contentTypeHeader->value, "application")) {
        http_response->contentType = Application;
    } else if (strcasestr(contentTypeHeader->value, "video")) {
        http_response->contentType = Video;
    } else {
        http_response->contentType = VND;
    }


    resp_current_pointer = resp_header_end_pointer + 4;
    http_response->body = calloc(1, http_response->bodyLen);
    if (!http_response->body) {
        perror("Error allocating memory for http response body");
        return 0;
    }
    memcpy(http_response->body, resp_current_pointer, http_response->bodyLen);
    return 1;
}