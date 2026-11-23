#pragma once

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    const char* val;
    bool is_yes; // true — ДА, false — НЕТ
} PathElem;

typedef struct {
    PathElem* data;
    size_t capacity;
    size_t free;
    size_t string_len;
} PathStack;

PathStack stack_initialization(size_t size);
void stack_push(PathStack* st, const char* val, bool is_yes);
void stack_print(PathStack* st);
void stack_destroy(PathStack* st);
