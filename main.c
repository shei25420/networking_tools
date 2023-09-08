#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "WebScraper/WebScraper.h"

int main () {
   WebScraper * scraper = web_scraper_constructor("https://physio4u.co", "./physio4u");
   scraper->init(scraper);
   web_scraper_deconstructor(scraper);
}