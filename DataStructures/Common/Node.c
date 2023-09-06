//
// Created by z3r0 on 8/22/23.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Node.h"

Node* node_constructor (void* data, int dataLen) {
    Node* node = calloc(1, sizeof(Node));
    if (!node) {
        fprintf(stderr, "[-] error allocating node structure\n");
        return NULL;
    }

    node->data = calloc(1, dataLen);
    if (!node->data) {
        fprintf(stderr, "[-] error allocating node structure data\n");
        free(node);
        return NULL;
    }
    memcpy(node->data, data, dataLen);

    node->dataLen = dataLen;
    return node;
}

void node_deconstructor (Node* node) {
    free(node->data);
    free(node);
}