//
// Created by z3r0 on 8/22/23.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "LinkedLists.h"

LinkedListNode* list_node_constructor (void* data, int dataLen);
void insert_list (int index, void* data, int dataLen, LinkedList* list);
void remove_list (int index, LinkedList* list);
void* search_list (int index, LinkedList* list);

LinkedList* linked_list_constructor() {
    LinkedList* list = (LinkedList*) calloc(1, sizeof(LinkedList));
    if (!list) {
        fprintf(stderr, "[-] error allocating linked list structure\n");
        return NULL;
    }

    list->insert = insert_list;
    list->remove = remove_list;
    list->search = search_list;

    return list;
}

void linked_list_deconstructor(LinkedList* list) {
    while (list->length) {
        remove_list(0, list);
    }
    fprintf(stderr, "[-] linked list destroyed\n");
}

LinkedListNode* list_node_constructor (void* data, int dataLen) {
    LinkedListNode* listNode = (LinkedListNode*)calloc(1, sizeof(LinkedListNode));
    if (!listNode) {
        fprintf(stderr, "[-] error allocating memory for linked list node\n");
        return NULL;
    }

    listNode->node = node_constructor(data, dataLen);
    if (!listNode->node) {
        free(listNode);
        return NULL;
    }

    return listNode;
}

void list_node_deconstructor (LinkedListNode* listNode) {
    node_deconstructor(listNode->node);
    free(listNode);
}

LinkedListNode* iterate_list (int index, LinkedList* list) {
    if (index < 0 || (index > list->length)) {
        fprintf(stderr, "[-] index is out of bounds\n");
        return NULL;
    } else if (index == 0) return list->head;

    LinkedListNode* cursor = list->head;
    while (index--) {
        cursor = cursor->next;
    };

    return cursor;
}

void insert_list (int index, void* data, int dataLen, LinkedList* list) {
    LinkedListNode* cursor = NULL;
    if (index < 0) {
        fprintf(stderr, "[-] index is out of range\n");
        return;
    }
    LinkedListNode* listNode = list_node_constructor(data, dataLen);
    if (!listNode) return;

    if (index == 0) {
        if (!list->head) list->head = listNode;
        else {
            cursor->next = list->head;
            list->head = cursor;
        }
    } else {
        cursor = iterate_list(index - 1, list);
        if (cursor) {
            listNode->next = cursor->next;
            cursor->next = listNode;
        } else {
            fprintf(stderr, "[-] error iterating list\n");
            return;
        }
    }

    list->length++;
}

void remove_list (int index, LinkedList* list) {
    LinkedListNode* delNode = NULL;
    if (index == 0) {
        delNode = list->head;
        list->head = list->head->next;
    } else {
        LinkedListNode* cursor = iterate_list(index - 1, list);
        delNode = cursor->next;
        cursor->next = cursor->next->next;
    }

    list_node_deconstructor(delNode);
    list->length--;
}

void* search_list (int index, LinkedList* list) {
    LinkedListNode* cursor = iterate_list(index, list);
    if (!cursor) return NULL;

    return cursor->node;
}