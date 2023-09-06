//
// Created by shei on 8/24/23.
//
#include <stdlib.h>
#include <stdio.h>
#include "BinaryNodeStack.h"

int push_stack (BinaryTreeNode* node, BinaryNodeStack* stack);
BinaryTreeNode *pop_stack (BinaryNodeStack* stack);

BinaryNodeStack * binary_node_stack_constructor (int capacity) {
    BinaryNodeStack * binary_node_stack = (BinaryNodeStack*)calloc(1, sizeof(BinaryNodeStack));
    if (!binary_node_stack) return NULL;

    binary_node_stack->capacity = 100;
    binary_node_stack->top = -1;
    binary_node_stack->nodeArr = (BinaryTreeNode**)calloc(binary_node_stack->capacity, sizeof(BinaryTreeNode*));

    binary_node_stack->push_stack = push_stack;
    binary_node_stack->pop_stack = pop_stack;
    return binary_node_stack;
}

void binary_node_stack_deconstructor(BinaryNodeStack* stack) {
    free(stack->nodeArr);
    free(stack);
}

int push_stack (BinaryTreeNode* node, BinaryNodeStack* stack) {
    int status = 0;
    printf("%d:%d\n", stack->top, stack->capacity);
    if (stack->top == (stack->capacity - 1)) {
        stack->capacity *= 2;
        BinaryTreeNode ** newArr = (BinaryTreeNode**)calloc(stack->capacity, sizeof(BinaryTreeNode*));
        if (!newArr) {
            perror("Error allocating memory for stack arr");
            goto exit;
        }
    }
    stack->nodeArr[++stack->top] = node;
    status = 1;
    exit:
    return status;
}

BinaryTreeNode *pop_stack (BinaryNodeStack* stack) {
    if (stack->top == -1) {
        fprintf(stderr, "[-] stack is empty\n");
        return 0;
    }
    return stack->nodeArr[stack->top--];
}