//
// Created by shei on 8/23/23.
//

#ifndef SCRAPER_HTMLLINKSPARSER_H
#define SCRAPER_HTMLLINKSPARSER_H

typedef void* (*link_process_func)(char* link, void* extra_args);
int parse_html_links(char* html, link_process_func, void* funcArgs);

#endif //SCRAPER_HTMLLINKSPARSER_H
