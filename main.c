#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "WebScraper/WebScraper.h"

int main () {
   WebScraper * scraper = web_scraper_constructor("https://mannatthemes.com/metrica/default/index.html", "./metrica");
   scraper->init(scraper);
   web_scraper_deconstructor(scraper);
}