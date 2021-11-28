//
// Created by IvanBrekman on 21.11.2021
//

#include "../libs/tree.h"

#define GET_LEFT_CHILD(new_node) {                                                          \
    LOG2(get_new_node_func_debug(data, new_node, "Getting left node"););                 \
    INDEX++;                                    \
    Node* left_child = get_new_node(data);                                   \
                                                                                            \
      new_node->left   = left_child;                                                        \
    left_child->parent = new_node;                                                          \
    LOG2(get_new_node_func_debug(data, new_node, "Adding left node"););                  \
}

#define GET_RIGHT_CHILD(new_node) {                                                         \
    LOG2(get_new_node_func_debug(data, new_node, "Getting right node"););                \
    INDEX++;                                \
    Node* right_child = get_new_node(data);                                  \
                                                                                            \
        new_node->right  = right_child;                                                     \
    right_child->parent = new_node;                                                         \
    LOG2(get_new_node_func_debug(data, new_node, "Adding right node"););                 \
}

#define DATA     data->data
#define INDEX    data->index
#define DATA_VAL DATA[INDEX]

const int  OPEN_BRACKET   = '(';
const int CLOSE_BRACKET   = ')';
const int VARIABLE_SYMBOL = 'x';

struct ParseContext {
    const char* data = NULL;
    int index = -1;
};

struct Functions {
    const char* name = NULL;
    const int   code = -1;
};

int print_node_val(Node* node);
const char* get_func_name(int func_code);

int read_tree_from_file(Tree* tree, const char* source_file);

Node* get_new_node(ParseContext* data);
int  get_node_data(ParseContext* data, int* data_store);
int get_new_node_func_debug(const ParseContext* data, Node* cur_node, const char* reason);

int  Tree_dump_graph(Tree* tree, const char* reason, FILE* log, int show_parent_edge=0);

