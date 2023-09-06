//
// Created by z3r0 on 8/22/23.
//

#ifndef SCRAPER_NODE_H
#define SCRAPER_NODE_H
typedef struct {
    int dataLen;
    void* data;
} Node;

Node* node_constructor (void* data, int dataLen);
void node_deconstructor (Node* node);
#endif //SCRAPER_NODE_H
