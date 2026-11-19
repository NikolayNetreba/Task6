#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

const char* file_name = "data.txt";

const size_t BUFFER_SIZE = 128;
#define BUF_SIZE "127"

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

#define DUMP(tree, idx)                                 \
    do {                                                \
        printf("==================================\n"); \
        text_dump(tree, tree.root);                     \
        printf("==================================\n"); \
    } while (0);

void text_dump(Tree tree, int idx) {
    if (idx == -1)
        return;
    printf("node: %s\n", tree.data[idx]);
    printf("left: %d, right: %d\n", tree.data[idx].left_child_idx, tree.data[idx].right_child_idx);

    text_dump(tree, tree.data[idx].left_child_idx);
    text_dump(tree, tree.data[idx].right_child_idx);
}

int build_tree(Tree* tree, const char** ptr) {
    assert(tree);
    assert(ptr);

    int offset = 0;

    char val_buffer[BUFFER_SIZE] = { };
    if (sscanf(*ptr, " (\"%" BUF_SIZE "[^\"]\"%n", val_buffer, &offset) == 1) {
        *ptr += offset;
        if (strcmp(val_buffer, "nil") == 0) {
            sscanf(*ptr, " )%n", &offset);
            *ptr += offset;
            return -1;
        }
        int current = tree->free;
        tree->free += 1;

        tree->data[current].val = strdup(val_buffer);
        tree->data[current].left_child_idx = build_tree(tree, ptr);
        tree->data[current].right_child_idx = build_tree(tree, ptr);

        offset = 0;
        sscanf(*ptr, " )%n", &offset);
        *ptr += offset;

        return current;
    }

    return -1;
}

void expand_tree(Tree* tree) {
    assert(tree);

    tree->capacity *= 1.5;
    tree->data = (Node*)realloc(tree->data, tree->capacity * sizeof(Node));
    assert(tree->data);
}

const char* open_and_read_from_file(const char* file_name) {
    assert(file_name);

    int fd = open(file_name, O_RDONLY);
    if (fd < 0) {
        perror("Error opening file");
        return NULL;
    }

    struct stat file_info = { };
    if (fstat(fd, &file_info) < 0) {
        perror("Error getting file info");
        close(fd);
        return NULL;
    }

    off_t size = file_info.st_size + 1;

    char* buffer = (char*)calloc(size, sizeof(char));
    if (!buffer) {
        perror("Error allocating memory");
        close(fd);
        return NULL;
    }

    if (read(fd, buffer, size) < 0) {
        perror("Error reading from file");
        close(fd);
        return NULL;
    }
    close(fd);

    return buffer;
}

void initialization(Tree* tree, const char** ptr) {
    assert(tree);
    assert(ptr);

    int offset = 0;
    *tree = { .free = 0, .capacity = 0 };

    sscanf(*ptr, "%d%n", &tree->capacity, &offset);
    tree->capacity += 10;
    *ptr += offset;

    tree->data = (Node*)calloc(tree->capacity, sizeof(Node));

    assert(tree->data);
}

typedef enum {
    GUESS_OP,
    DEFINITION_OP,
    COMPARE_OP,
    SHOW_OP,
    SAVE_OP,
    NO_SAVE_OP
} UserOp;

UserOp get_state() {
    char input[BUFFER_SIZE] = { };

    while (1) {
        printf("Что вы желаете: [о]тгадать, [д]ать определение,\n[с]равнить объекты, [п]оказать дерево, [в]ыйти с сохранением или [б]ез него?\n");

        scanf(" %" BUF_SIZE "[^\n]", input);

        if (strcmp(input, "о") == 0)
            return GUESS_OP;
        if (strcmp(input, "д") == 0)
            return DEFINITION_OP;
        if (strcmp(input, "с") == 0)
            return COMPARE_OP;
        if (strcmp(input, "п") == 0)
            return SHOW_OP;
        if (strcmp(input, "в") == 0)
            return SAVE_OP;
        if (strcmp(input, "б") == 0)
            return NO_SAVE_OP;

        printf("Попробуй еще раз)\n\n");
    }
}

bool ask_que(const char* que) {
    char input[BUFFER_SIZE] = { };

    while (1) {
        printf("это %s?\n", que);

        scanf(" %" BUF_SIZE "[^\n]", input);

        if (strcmp(input, "да") == 0)
            return true;
        if (strcmp(input, "нет") == 0)
            return false;

        printf("Попробуй еще раз)\n\n");
    }

    return false;
}

void add_new_node(Tree* tree, int old_node_idx) {
    assert(tree);
    Node* old_node = &tree->data[old_node_idx];

    if (tree->free == tree->capacity)
        expand_tree(tree);

    printf("Кто же это?\n");

    char buffer1[BUFFER_SIZE] = { };
    scanf(" %" BUF_SIZE "[^\n]", buffer1);

    printf("Чем %s отличается от %s?\n", buffer1, old_node->val);
    char buffer2[BUFFER_SIZE] = { };
    scanf(" %" BUF_SIZE "[^\n]", buffer2);

    const char* temp = old_node->val;
    old_node->val = strdup(buffer2);

    old_node->left_child_idx = tree->free;
    tree->data[tree->free].val = strdup(buffer1);
    tree->data[tree->free].left_child_idx = -1;
    tree->data[tree->free].right_child_idx = -1;

    tree->free++;

    old_node->right_child_idx = tree->free;
    tree->data[tree->free].val = temp;
    tree->data[tree->free].left_child_idx = -1;
    tree->data[tree->free].right_child_idx = -1;
}

void guess_word(Tree* tree, int idx) {
    assert(tree);

    bool is_it_yes = ask_que(tree->data[idx].val);
    if (is_it_yes) {
        if (tree->data[idx].left_child_idx == -1) {
            printf("Я же говорил!\n");
        } else {
            guess_word(tree, tree->data[idx].left_child_idx);
        }
    } else {
        if (tree->data[idx].right_child_idx == -1) {
            add_new_node(tree, idx);
        } else {
            guess_word(tree, tree->data[idx].right_child_idx);
        }
    }
}

void dump_node_dot(FILE* file, const Tree* tree, int idx) {
    if (idx == -1) {
        return;
    }

    const Node* node = &tree->data[idx];

    fprintf(file, "    node_%d [label=\"\\\"%s\\\"\"];\n", idx, node->val);

    if (node->left_child_idx != -1) {
        fprintf(file, "    node_%d -> node_%d [label=\" ДА\", color=\"#44C744\", fontcolor=\"#44C744\"];\n",
            idx, node->left_child_idx);
        dump_node_dot(file, tree, node->left_child_idx);
    }

    if (node->right_child_idx != -1) {
        fprintf(file, "    node_%d -> node_%d [label=\" НЕТ\", color=\"#C828B8\", fontcolor=\"#C828B8\"];\n",
            idx, node->right_child_idx);
        dump_node_dot(file, tree, node->right_child_idx);
    }
}

void show_tree(Tree* tree) {
    assert(tree);

    FILE* file = fopen("graph.dot", "w");
    assert(file);

    fprintf(file, "digraph Tree {\n");
    fprintf(file, "    rankdir=TB;\n");
    fprintf(file, "    splines=line;\n");
    fprintf(file, "    node [shape=box, style=\"rounded,filled\", fillcolor=\"#E6F3FF\", color=\"#4A90E2\", fontcolor=\"#000000\", penwidth=1.5];\n");
    fprintf(file, "    edge [penwidth=1.2];\n\n");

    dump_node_dot(file, tree, tree->root);

    fprintf(file, "}\n");
    fclose(file);

    system("dot -Tpng graph.dot -o graph.png");
    system("xdg-open graph.png &");
}

void save_node(const Tree* tree, int idx, FILE* file) {
    assert(tree);
    assert(file);

    if (idx == -1) {
        fprintf(file, "(\"nil\")");
        return;
    }

    fprintf(file, "(\"%s\" ", tree->data[idx].val);
    save_node(tree, tree->data[idx].left_child_idx, file);
    fprintf(file, " ");
    save_node(tree, tree->data[idx].right_child_idx, file);
    fprintf(file, ")");
}

void save_exit(Tree* tree) {
    assert(tree);

    FILE* file = fopen(file_name, "w");
    assert(file);

    fprintf(file, "%d ", tree->free);

    save_node(tree, tree->root, file);

    fclose(file);
}

void handle_op(Tree* tree, UserOp op) {
    assert(tree);

    switch (op) {
    case GUESS_OP:
        guess_word(tree, tree->root);
        break;
    case DEFINITION_OP:
        break;
    case SHOW_OP:
        show_tree(tree);
        break;
    case SAVE_OP:
        save_exit(tree);
        break;
    case NO_SAVE_OP:
        break;
    default:
        break;
    }
}

int main(int argc, char* argv[]) {
    if (argc == 2) {
        file_name = argv[1];
    }

    const char* ptr = open_and_read_from_file(file_name);
    if (!ptr) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    Tree tree = { };
    initialization(&tree, &ptr);

    tree.root = build_tree(&tree, &ptr);

    DUMP(tree, tree.root);

    UserOp op = NO_SAVE_OP;
    do {
        op = get_state();
        handle_op(&tree, op);
    } while (op != NO_SAVE_OP && op != SAVE_OP);

    return 0;
}
