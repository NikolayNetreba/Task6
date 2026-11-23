#include "PathStack.h"

PathStack stack_initialization(size_t size) {
    assert(size > 0);

    PathStack st = {
        .data = (PathElem*)calloc(size, sizeof(PathElem)),
        .capacity = size,
        .free = 0,
        .string_len = 0
    };
    assert(st.data);

    return st;
}

void expand_stack(PathStack* st) {
    assert(st);

    st->capacity *= 2;
    PathElem* new_data = (PathElem*)realloc(st->data, st->capacity * sizeof(PathElem));
    assert(new_data);
    st->data = new_data;
}

void stack_push(PathStack* st, const char* val, bool is_yes) {
    assert(st);
    assert(val);

    if (st->capacity == st->free)
        expand_stack(st);

    st->data[st->free].val = val;
    st->data[st->free].is_yes = is_yes;
    st->free++;

    st->string_len += strlen(val) + (is_yes ? 0 : strlen("не "));
}

void stack_print(PathStack* st) {
    assert(st);

    size_t buf_size = st->string_len * 2;
    char* res = (char*)calloc(buf_size, sizeof(char));
    assert(res);

    size_t shift = 0;

    for (int i = (int)st->free - 1; i >= 0; i--) {
        if (!st->data[i].is_yes) {
            shift += sprintf(res + shift, "не ");
        }
        shift += sprintf(res + shift, "%s", st->data[i].val);

        if (i > 0) {
            shift += sprintf(res + shift, ", ");
        }
    }

    printf("%s\n", res);

    free(res);
}

void stack_destroy(PathStack* st) {
    assert(st);

    free(st->data);
    st->data = NULL;
    st->free = 0;
    st->capacity = 0;
}
