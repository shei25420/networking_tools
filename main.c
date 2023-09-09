#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "WebScraper/WebScraper.h"
#include "IPLookup/IPLookup.h"

//int main () {
//   WebScraper * scraper = web_scraper_constructor("https://physio4u.co", "./physio4u");
//   scraper->init(scraper);
//   web_scraper_deconstructor(scraper);
//}

int main () {
    IPLookup *ipLookup = ip_lookup_constructor("google.com");
    if (!ipLookup) return 1;

    ipLookup->lookup(ipLookup);
    ip_lookup_deconstructor(ipLookup);
    return 0;
}
