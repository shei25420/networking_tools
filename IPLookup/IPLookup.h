//
// Created by shei on 9/9/23.
//

#ifndef SCRAPER_IPLOOKUP_H
#define SCRAPER_IPLOOKUP_H

typedef struct ip_lookup {
    char* hostname;

    void (*lookup)(struct ip_lookup* ip_lookup);
} IPLookup;

IPLookup * ip_lookup_constructor (char* hostname);
void ip_lookup_deconstructor(IPLookup* ip_lookup);
#endif //SCRAPER_IPLOOKUP_H
