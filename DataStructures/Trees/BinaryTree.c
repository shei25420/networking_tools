
//
// Created by z3r0 on 8/22/23.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "BinaryTree.h"

void insert_tree (void* data, int dataLen, BinaryTree* tree);
void remove_tree (void* data, BinaryTree* tree);
void* search_tree (void* data, BinaryTree* tree);
void destroy_tree (BinaryTreeNode* node);
void process_all (BinaryTree* tree, void (*processor)(void* data, int data_len));
int str_cmp(void* data1, void* data2);

BinaryTree* binary_tree_constructor(int (*cmp)(void* data, void* data1)) {
    BinaryTree* tree = (BinaryTree*)calloc(1, sizeof(BinaryTree));
    if (!tree) {
        fprintf(stderr, "[-] error allocating memory for binary tree\n");
        return NULL;
    }

    tree->insert = insert_tree;
    tree->remove = remove_tree;
    tree->search = search_tree;
    tree->cmp = cmp;
    tree->process_all = process_all;

    return tree;
}

void binary_tree_deconstructor(BinaryTree* tree) {
    destroy_tree(tree->root);
    free(tree);
}

void destroy_tree (BinaryTreeNode* node) {
    if (node == NULL) return;

    destroy_tree(node->left);
    destroy_tree(node->right);

    free(node);
}

BinaryTreeNode* binary_tree_node_constructor (void* data, int dataLen) {
    BinaryTreeNode* node = (BinaryTreeNode*) calloc(1, sizeof(BinaryTreeNode));
    if (!node) {
        fprintf(stderr, "[-] error allocating memory for tree node\n");
        return NULL;
    }

    node->node = node_constructor(data, dataLen);
    return node;
}

void binary_tree_node_deconstructor (BinaryTreeNode* node) {
    node_deconstructor(node->node);
    free(node);
}

void process_tree (BinaryTreeNode* root, void (*process)(void* data, int data_len)) {
    if (root == NULL) return;

    process(root->node->data, root->node->dataLen);

    process_tree(root->left, process);
    process_tree(root->right, process);
}

void process_all (BinaryTree* tree, void (*processor)(void* data, int data_len)) {
    process_tree(tree->root, processor);
}

BinaryTreeNode* findMininumNode (BinaryTreeNode* root) {
    while (root->left) {
        root = root->left;
    }
    return root;
}

BinaryTreeNode* findNodeInorderSuccessor (BinaryTreeNode* root) {
    if (!root->right) return NULL;

    BinaryTreeNode* inorderSuccessor = findMininumNode(root->right);
    return inorderSuccessor;
}

BinaryTreeNode* iterate_tree (void* data, BinaryTreeNode* cursor, int* direction, BinaryTree* tree) {
    if (!cursor) return NULL;

    int cmp = tree->cmp(data, cursor->node->data);
    if ((cmp < 0)) {
        *direction = -1;
        if (cursor->left) {
            cursor = iterate_tree(data, cursor->left, direction, tree);
        };
    } else if ((cmp > 0)) {
        *direction = 1;
        if (cursor->right) {
            cursor = iterate_tree(data, cursor->right, direction, tree);
        }
    } else if (cmp == 0) {
        *direction = 0;
    }

    return cursor;
}

void insert_tree (void* data, int dataLen, BinaryTree* tree) {
    BinaryTreeNode* newNode = binary_tree_node_constructor(data, dataLen);
    if (!newNode) return;

    if (!tree->root) {
        tree->root = newNode;
        goto exit;
    }

    int direction = 99;
    BinaryTreeNode* cursor = iterate_tree(data, tree->root, &direction, tree);
    if (direction == 0) {
        // fprintf(stderr, "[-] item already exists\n");
        return;
    }

    if (direction == 1) {
        newNode->parent = cursor;
        cursor->right = newNode;
    } else if (direction == -1) {
        newNode->parent = cursor;
        cursor->left = newNode;
    }

    exit:
    tree->total_nodes++;
}

void remove_tree (void* data, BinaryTree* tree) {
    int direction = 99;
    BinaryTreeNode* cursor = iterate_tree(data, tree->root, &direction, tree), *cursor_to_delete = cursor;
    if (direction != 0) {
        fprintf(stderr, "[-] item not found in tree\n");
        return;
    }

    int cmp = tree->cmp(cursor->node->data, cursor->parent->node->data);
    if (cursor->left == NULL && cursor->right == NULL) {
        if ((cmp < 0)) {
            cursor->parent->left = NULL;
        } else if ((cmp > 0)) {
            cursor->parent->right = NULL;
        }
    } else if (cursor->left && cursor->right) {
        BinaryTreeNode* successor = findNodeInorderSuccessor(cursor);
        if ((cmp < 0)) {
            cursor->parent->left = successor;
        } else if ((cmp > 0)) {
            cursor->parent->right = successor;
        }
    } else if (cursor->left) {
        if ((cmp < 0)) {
            cursor->parent->left = cursor->left;
        } else if ((cmp > 0)) {
            cursor->parent->right = cursor->left;
        }
    } else if (cursor->right) {
        if ((cmp < 0)) {
            cursor->parent->left = cursor->right;
        } else if ((cmp > 0)) {
            cursor->parent->right = cursor->right;
        }
    }

    binary_tree_node_deconstructor(cursor_to_delete);
    tree->total_nodes--;
}

void* search_tree (void* data, BinaryTree* tree) {
    int direction = 99;
    BinaryTreeNode* cursor = iterate_tree(data, tree->root, &direction, tree);

    if (direction == 0)return cursor->node->data;
    return NULL;
}

int str_cmp(void* data1, void* data2) {
    int cmp = strcmp((char *)data1, (char*)data2);
    if (cmp < 0) return -1;
    else if (cmp > 0) return 1;

    return 0;
}

