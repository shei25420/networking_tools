//
// Created by shei on 8/23/23.
//
#define _GNU_SOURCE         /* See feature_test_macros(7) */
#include <libxml/HTMLparser.h>
#include <regex.h>
#include <string.h>
#include "HTMLLinksParser.h"

const char* href_pattern = "href=[\"'][a-zA-Z0-9]([^\"']+)";
const char* script_pattern = "src=[\"'][a-zA-Z0-9]([^\"']+)";

void process_node (xmlNode* node, link_process_func func, void* funcArgs);
void extractLink (const char* text, const char* pattern, link_process_func func, void* funcArgs);
int parse_html_links(char* html, link_process_func func, void* funcArgs);

void extractLink (const char* text, const char* pattern, link_process_func func, void* funcArgs) {
    if (text[0] == '#' || strcasestr(text, "javascript") || strcasestr(text, "mail") || strcasestr(text, "tel")) {
        return;
    }

    func(text, funcArgs);
}

void process_node (xmlNode* node, link_process_func func, void* funcArgs) {
    if (!node) return;

    if (node->type == XML_ELEMENT_NODE) {
        xmlChar * linkText = NULL;
        if ((strcasecmp((char *)node->name, "a") == 0) || (strcasecmp((char *)node->name, "link") == 0)) {
            //Iterate through the element properties
            xmlAttr * properties = node->properties;
            while (properties) {
                if (strcasecmp(properties->name, "href") == 0) {
                    linkText = xmlNodeListGetString(properties->doc, properties->xmlChildrenNode, 1);
                    break;
                }
                properties = properties->next;
            }


            if (linkText) extractLink(linkText, href_pattern, func, funcArgs);
        } else if ((xmlStrcasecmp(node->name, (const xmlChar*)"script") == 0)) {
            //Iterate through the element properties
            xmlAttr * properties = node->properties;
            while (properties) {
                if (strcasecmp(properties->name, "src") == 0) {
                    linkText = xmlNodeListGetString(properties->doc, properties->xmlChildrenNode, 1);
                    break;
                }
                properties = properties->next;
            }
            if (linkText) extractLink(linkText, script_pattern, func, funcArgs);
        }

        if (linkText) {
            xmlFree(linkText);
        }
    }
}

int parse_html_links(char* html, link_process_func func, void* funcArgs) {
    htmlDocPtr doc = htmlReadDoc(BAD_CAST html, NULL, NULL, HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING | HTML_PARSE_NOBLANKS);
    if (!doc) {
        perror("Error reading html");
        return 0;
    }

    xmlNode *node = xmlDocGetRootElement(doc);

    xmlNode *stack[100];
    int stackTop = 0;
    stack[stackTop++] = node;

    while (stackTop > 0) {
        xmlNode *currNode = stack[--stackTop];
        process_node(currNode, func, funcArgs);

        xmlNode* child = NULL;
        for (child = currNode->children; child; child = child->next) {
            stack[stackTop++] = child;
        }

    }

    return 1;
}