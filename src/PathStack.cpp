#include "PathStack.h"

PathStack stack_initialization(size_t size) {
    assert(size > 0);

    PathStack st = {
        .data = (bool*)calloc(size, sizeof(bool)),
        .capacity = size,
        .free = 0
    };
    assert(st.data);

    return st;
}

static void expand_stack(PathStack* st) {
    assert(st);

    st->capacity *= 2;
    bool* new_data = (bool*)realloc(st->data, st->capacity * sizeof(bool));
    assert(new_data);
    st->data = new_data;
}

void stack_push(PathStack* st, bool is_yes) {
    assert(st);

    if (st->capacity == st->free) {
        expand_stack(st);
    }

    st->data[st->free] = is_yes;
    st->free++;
}

char* build_path_string(Tree* tree, int start_idx, PathStack* st, int from_pos, int to_pos) {
    assert(tree);
    assert(st);

    size_t cap = 128;
    size_t len = 0;
    char* buf = (char*)calloc(cap, sizeof(char));
    assert(buf);

    int curr = start_idx;
    bool first = true;

    for (int i = from_pos; i >= to_pos; i--) {
        assert(curr != -1);
        bool is_yes = st->data[i];

        const char* prefix = first ? "" : ", ";
        const char* neg = is_yes ? "" : "не ";
        const char* val = tree->data[curr].val;

        size_t need = strlen(prefix) + strlen(neg) + strlen(val) + 1;
        while (len + need > cap) {
            cap *= 2;
            char* grown = (char*)realloc(buf, cap);
            assert(grown);
            buf = grown;
        }

        len += (size_t)sprintf(buf + len, "%s%s%s", prefix, neg, val);
        first = false;

        curr = is_yes ? tree->data[curr].left_child_idx : tree->data[curr].right_child_idx;
    }

    return buf;
}

void stack_print(Tree* tree, int start_idx, PathStack* st, int from_pos, int to_pos) {
    assert(tree);
    assert(st);

    char* line = build_path_string(tree, start_idx, st, from_pos, to_pos);
    printf("%s\n", line);

    // Здесь же удобно вызвать функцию синтеза речи: speak_text(line);
    free(line);
}

void stack_destroy(PathStack* st) {
    assert(st);

    free(st->data);
    st->data = NULL;
    st->free = 0;
    st->capacity = 0;
}
