//
// Created by shei on 9/9/23.
//

#ifndef SCRAPER_DNSQUERY_H
#define SCRAPER_DNSQUERY_H
typedef struct dns_query {
    unsigned char* (*print_name)(unsigned char* msg, unsigned char* p, unsigned char* end);
    void (*print_full_dns_message)(char* message, int messageLen);
} DNSQuery;

DNSQuery * dns_query_constructor();
void dns_query_deconstructor(DNSQuery* dnsQuery);
#endif //SCRAPER_DNSQUERY_H
