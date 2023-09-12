//
// Created by shei on 8/23/23.
//

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include "WebScraper.h"
#include "../Protocols/HttpClient.h"
#include "../System/ThreadPool.h"
#include "../Protocols/HTTP/HTTPRequest.h"
#include "../Protocols/HTTP/HTTPResponse.h"
#include "./HTMLLinksParser.h"
#include "./BinaryNodeStack.h"

typedef struct link_args {
    char* request;
    WebScraper *scraper;
} LinkArgs;

void process_links (HTTPClient *client, WebScraper* scraper);
int scrape_link (void* link, int linkLen, HTTPClient* client, WebScraper* scraper);
long parse_url (char *url, WebScraper* scraper);
int init_scraper (WebScraper* scraper);
int saveToFile (void* body, long bodySize, char* path, char* dirPath);
void* insert_link (char* link, void* args);

WebScraper * web_scraper_constructor (char* url, char* dirPath) {
    WebScraper * scraper = (WebScraper*) calloc(1, sizeof(WebScraper));
    scraper->links = binary_tree_constructor(str_cmp);
    if (!scraper->links) return NULL;
    if (!parse_url(url, scraper)) return NULL;

    scraper->dirPath = (char *)calloc(1, (strlen(dirPath) + 1));
    if (!scraper->dirPath) {
        perror("[-] error allocating dirPath memory");
        binary_tree_deconstructor(scraper->links);
        free(scraper->host);
        free(scraper->port);
        free(scraper);
        return NULL;
    }
    strcpy(scraper->dirPath, dirPath);

    scraper->init = init_scraper;
    return scraper;
}

void web_scraper_deconstructor (WebScraper *scraper) {
    free(scraper->host);
    free(scraper->port);
    free(scraper);
}

int init_scraper (WebScraper* scraper) {
    int status = 0;
//    Initialize Thread Pool for scraper
    ThreadPool* pool = thread_pool_constructor(0);
    if (!pool) return status;

    HTTPClient* httpClient = http_client_constructor(scraper->host, scraper->port);
    if (!httpClient) return status;

    if (!httpClient->connect(httpClient)) return status;

    process_links(httpClient, scraper);

    http_client_deconstructor(httpClient);
    thread_pool_deconstructor(pool);
    return status;
}

void process_links (HTTPClient *client, WebScraper* scraper) {
    if (!(scraper->links->total_nodes)) {
        fprintf(stderr, "[-] no links to process");
        return;
    }
    BinaryNodeStack *stack = binary_node_stack_constructor(100);
    if (!stack) return;

    stack->push_stack(scraper->links->root, stack);
    while (stack->top != -1) {
        BinaryTreeNode * currentNode = stack->pop_stack(stack);
        scrape_link((char *)currentNode->node->data, currentNode->node->dataLen, client, scraper);

        if (currentNode->left != NULL) {
            stack->push_stack(currentNode->left, stack);
        }

        if (currentNode->right != NULL) {
            stack->push_stack(currentNode->right, stack);
        }

    }
    binary_node_stack_deconstructor(stack);
}

int scrape_link (void* link, int linkLen, HTTPClient* client, WebScraper* scraper) {
    int status = 0;
    HTTPRequest *request = http_request_constructor((char *)link);
    if (!request) goto exit;

    HTTPResponse *response = http_response_constructor();
    if (!response) goto fail1;
    retry:
    response->retry = 0;
    if (request->send_request(GET, NULL, NULL, client, request)) {
        if (response->recv_response(client, response)) {
            if (response->retry) {
                client->reconnect(client);
                goto retry;
            } else if ((response->statusCode < 200) || (response->statusCode >= 400)) {
                  fprintf(stderr, "[+] Error fetching response from request %s with status code: %ld %s\n", request->req_path, response->statusCode, response->statusText);
                  goto fail2;
            } else if ((response->contentType != Application) && (response->statusCode == 200)) {
                LinkArgs args = { 0  };
                args.request = request->req_path;
                args.scraper = scraper;

                parse_html_links(response->body, insert_link, &args);
                saveToFile(response->body, response->bodyLen, link, scraper->dirPath);
                printf("[+] done writing file: %s : %lu bytes\n", (char *)link, response->bodyLen);
            }
        }
    }

    status = 1;
    fail2:
    http_response_deconstructor(response);
    fail1:
    http_request_deconstructor(request);
    exit:
    return status;
}

int generate_folder_structure (char* fullPath) {
    int status = 0;
    long long dirPathOffset = 0;

    char* current_dir_pointer = fullPath, *dir_end_pointer = 0, *dir_start_pointer = 0;
    if (current_dir_pointer[0] == '.' && current_dir_pointer[1] == '/') current_dir_pointer += 2;

    while ((dir_end_pointer = strchr((current_dir_pointer + dirPathOffset), '/'))) {
        dirPathOffset = dir_end_pointer - current_dir_pointer;

        dir_start_pointer = current_dir_pointer + dirPathOffset;
        if (dir_start_pointer[0] == '.' && dir_start_pointer[1] == '/') {
            dirPathOffset += 2;
            continue;
        } else if ((strncmp(dir_start_pointer, "..", strlen("..")) == 0)) {
            dirPathOffset += strlen("../");
            continue;
        }
        char* dir = (char *)calloc(1, (dirPathOffset + 1));
        if (!dir) {
            perror("[+] error allocating memory for dir");
            goto exit;
        }

        strncpy(dir, current_dir_pointer, dirPathOffset);
        if (mkdir( dir, S_IRWXU) == -1) {
            if (errno != EEXIST) {
                perror("Error creating directory");
                free(dir);
                goto exit;
            }
        }
        free(dir);
        dirPathOffset++;
    }

    status = 1;
    exit:
    return status;
}

int saveToFile (void* body, long bodySize, char* path, char* dirPath) {
    int status = 0;
    unsigned long full_path_len = ((strcmp(path, "/") == 0) ? strlen("/index.html") : strlen(path)) + strlen(dirPath);
    char* full_path = (char *)calloc(1, full_path_len + 1), *q = 0, *dir = 0;
    if (!full_path) {
        perror("Error allocating full path memory");
        goto fail1;
    }


    strncpy(full_path, dirPath, full_path_len);
    strcat(full_path, ((strcmp(path, "/") == 0) ? "/index.html" : path));

    if(!generate_folder_structure(full_path)) {
        goto exit;
    }

    //Create FIle
    FILE* fd = fopen(full_path, "w+");
    if (!fd) {
        fprintf(stderr, "[-] error opening file: %s\n", full_path);
        perror("Error: ");
        goto fail1;
    }

    size_t bytesWritten = fwrite(body, 1, bodySize, fd);
    if (bytesWritten != bodySize) {
        perror("Error writing file");
        goto exit;
    }
    printf("[+] done writing to: %s: %lu bytes\n", full_path, bodySize);
    status = 1;
    exit:
        fclose(fd);
        free(full_path);
    fail1:
        return status;
}

void* insert_link (char* link, void* args) {
    LinkArgs *linkArgs = (LinkArgs*)args;

    if ((strncmp(link, "http://", strlen("http://")) == 0)) {
        link += 6;
    } else if ((strncmp(link, "https://", strlen("https://")) == 0)) {
        link += 7;
    }

    unsigned long long fullPathLen = 0;
    unsigned long long requestLen = 0;
    if (link[0] == '/') {
        fullPathLen = strlen(link);
    } else {
        //Get request path length
        char* z = strrchr(linkArgs->request, '/');
        if (z) requestLen = (z - linkArgs->request) + 1;
        if (requestLen == 0) requestLen = 1;

        fullPathLen += requestLen + strlen(link);
    }

    //Generate Full request
    char* fullPath = (char *)calloc(1, fullPathLen + 1);
    if (!fullPath) {
        perror("[-] error allocating fullPath memory");
        return NULL;
    }
    if (link[0] != '/') {
        strncpy(fullPath, linkArgs->request, requestLen);
        strncat(fullPath, link, strlen(link));
    } else {
        strncpy(fullPath, link, fullPathLen);
    }

    if (strstr(fullPath, "html1")) {
        printf("WTF!!");
    }
    linkArgs->scraper->links->insert(fullPath, strlen(fullPath), linkArgs->scraper->links);
    free(fullPath);
    return NULL;
}

long parse_url (char *url, WebScraper* scraper) {
    long status = 0, port_len = 0, host_len = 0, path_len;
    ScraperProtocol scraperProtocol;
    char* current_url_offset = url, *z = 0;
    if ((z = strstr(current_url_offset, "http://"))) {
        port_len = 2;
        scraperProtocol = HTTP;
        current_url_offset = z + 7;
    } else if ((z = strstr(current_url_offset, "https://"))) {
        port_len = 3;
        scraperProtocol = HTTPS;
        current_url_offset = z + 8;
    }

    scraper->port = (char *)calloc(1, port_len + 1);
    if (!scraper->port) {
        perror("Error allocating scraper port memory");
        goto exit;
    }
    strcpy(scraper->port, scraperProtocol == HTTP ? "80" : "443");

    if ((z = strstr(current_url_offset, "/"))) {
        host_len = z - current_url_offset;
        path_len = (long)strlen(z);
    } else {
        host_len = (long)strlen(current_url_offset);
        path_len = 1;
    }

    scraper->host = (char *)calloc(1, host_len + 1);
    if (!scraper->host) {
        perror("Error allocating scraper host memory");
        goto fail1;
    }
    strncpy(scraper->host, current_url_offset, host_len);
    if (z) current_url_offset += host_len;

    scraper->links->insert(path_len == 1 ? "/" : current_url_offset, (int)path_len, scraper->links);
    status = 1;
    goto exit;

    fail1:
    free(scraper->port);
    exit:
    return status;
}
