//
// Created by IvanBrekman on 21.11.2021
//

#include "../libs/tree.h"

#define GET_LEFT_CHILD(new_node) {                                                          \
    LOG2(get_new_node_func_debug(data, i, new_node, "Getting left node"););                 \
    *shift = 0;                                                                             \
    Node* left_child = get_new_node(&data[i + 1], shift);                                   \
                                                                                            \
      new_node->left   = left_child;                                                        \
    left_child->parent = new_node;                                                          \
    i += *shift;                                                                            \
    LOG2(get_new_node_func_debug(data, i, new_node, "Adding left node"););                  \
}

#define GET_RIGHT_CHILD(new_node) {                                                         \
    LOG2(get_new_node_func_debug(data, i, new_node, "Getting right node"););                \
    *shift = 0;                                                                             \
    Node* right_child = get_new_node(&data[i + 1], shift);                                  \
                                                                                            \
        new_node->right  = right_child;                                                     \
    right_child->parent = new_node;                                                         \
    i += *shift;                                                                            \
    LOG2(get_new_node_func_debug(data, i, new_node, "Adding right node"););                 \
}

const int  OPEN_BRACKET   = '(';
const int CLOSE_BRACKET   = ')';
const int VARIABLE_SYMBOL = 'x';

int read_tree_from_file(Tree* tree, const char* source_file);

Node* get_new_node(char* data, int* shift);
int get_node_data(char* data, int* data_store, int* shift);
int get_new_node_func_debug(const char* data, int index, Node* cur_node, const char* reason);

