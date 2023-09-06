//
// Created by z3r0 on 8/22/23.
//

#ifndef SCRAPER_LINKEDLISTS_H
#define SCRAPER_LINKEDLISTS_H
#include "../Common/Node.h"

typedef struct linked_list_node {
    Node* node;
    struct linked_list_node* next;
} LinkedListNode;

typedef struct linked_list {
    int length;
    LinkedListNode * head;
    void (*insert)(int index, void* data, int dataLen, struct linked_list* list);
    void (*remove)(int index, struct linked_list* list);
    void* (*search)(int index, struct linked_list* list);
} LinkedList;

LinkedList* linked_list_constructor();
void linked_list_deconstructor(LinkedList* list);

#endif //SCRAPER_LINKEDLISTS_H
