//
// Created by z3r0 on 8/22/23.
//
#ifndef SCRAPER_BINARYTREE_H
#define SCRAPER_BINARYTREE_H
#include "../Common/Node.h"

typedef struct binary_tree_node {
    Node* node;
    struct binary_tree_node* left;
    struct binary_tree_node* right;
    struct binary_tree_node* parent;
} BinaryTreeNode;

typedef struct binary_tree {
    BinaryTreeNode* root;
    int total_nodes;
    void (*insert)(void* data, int dataLen, struct binary_tree* tree);
    void (*remove)(void* data, struct binary_tree* tree);
    void* (*search)(void* data, struct binary_tree* tree);
    void (*process_all)(struct binary_tree* tree, void (*processor)(void* data, int data_len));
    int (*cmp)(void* data, void* data1); //Should return 1 when data is greater than data1, -1 when data is less than data1 and 0 when both are equal
} BinaryTree;

int str_cmp(void* data1, void* data2);

BinaryTree* binary_tree_constructor(int (*cmp)(void* data, void* data1));
void binary_tree_deconstructor(BinaryTree* tree);

// Helpers
int str_cmp (void* data1, void* data2);
#endif //SCRAPER_BINARYTREE_H
