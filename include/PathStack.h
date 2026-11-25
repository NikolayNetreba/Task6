#pragma once

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    bool* data;
    size_t capacity;
    size_t free;
} PathStack;

typedef struct {
    const char* val;
    int left_child_idx;
    int right_child_idx;
} Node;

typedef struct {
    Node* data;
    int root;
    int free;
    int capacity;
} Tree;

PathStack stack_initialization(size_t size);
void stack_push(PathStack* st, bool is_yes);
void stack_print(Tree* tree, int start_idx, PathStack* st, int from_pos, int to_pos);
void stack_destroy(PathStack* st);
