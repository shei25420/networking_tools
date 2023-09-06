//
// Created by shei on 8/24/23.
//

#ifndef SCRAPER_BINARYNODESTACK_H
#define SCRAPER_BINARYNODESTACK_H

#include "../DataStructures/Trees/BinaryTree.h"

typedef struct binary_node_stack{
    BinaryTreeNode ** nodeArr;
    int top;
    int capacity;

    int (*push_stack)(BinaryTreeNode* node, struct binary_node_stack* stack);
    BinaryTreeNode* (*pop_stack)(struct binary_node_stack* stack);
} BinaryNodeStack;

BinaryNodeStack * binary_node_stack_constructor (int capacity);
void binary_node_stack_deconstructor(BinaryNodeStack* stack);
#endif //SCRAPER_BINARYNODESTACK_H
