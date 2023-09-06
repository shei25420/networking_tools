//
// Created by shei on 8/23/23.
//

#ifndef SCRAPER_WEBSCRAPER_H
#define SCRAPER_WEBSCRAPER_H

#include "../DataStructures/Trees/BinaryTree.h"

typedef enum scraper_protocol {
    HTTP,
    HTTPS,
    FTP,
    STMP,
    IMAP
} ScraperProtocol;

typedef struct web_scraper {
    char* dirPath;
    char* host;
    char* port;

    BinaryTree* links;
    int (*init)(struct web_scraper* scraper);
} WebScraper;

WebScraper * web_scraper_constructor(char* url, char* dirPath);
void web_scraper_deconstructor(WebScraper* scraper);

#endif //SCRAPER_WEBSCRAPER_H
