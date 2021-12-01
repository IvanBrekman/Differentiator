//
// Created by IvanBrekman on 29.11.2021
//

#include "../libs/tree.h"

#define BIND_WITH_PARENT(prev_child, new_child) {                                       \
    new_child->parent = prev_child->parent;                                             \
    if      (is_left_child(prev_child))  prev_child->parent->left  = new_child;         \
    else if (is_right_child(prev_child)) prev_child->parent->right = new_child;         \
}

#define UPDATE_DATA {                                       \
    if (cur_node == tree->root) tree->root = node;          \
    BIND_WITH_PARENT(cur_node, node);                       \
    node_dtor(cur_node);                                    \
    s_amount++;                                             \
}

#define ADD_CHILDRENS(list, node) {                                                     \
    if (VALID_PTR(node->left))  list.push_back(node->left);                             \
    if (VALID_PTR(node->right)) list.push_back(node->right);                            \
}

#define LCONST(val) (cur_node->left-> data.type == data_type::CONST_T && cur_node->left-> data.value == val)
#define RCONST(val) (cur_node->right->data.type == data_type::CONST_T && cur_node->right->data.value == val)

#define CHECK_TYPE_AND_VAL(node, type_, val) node->data.type == type_ && node->data.value == val
#define CHECK_OPP(opp) CHECK_TYPE_AND_VAL(cur_node, data_type::OPP_T, opp)

int simplify(Tree* tree);

int simplify_constants     (Tree* tree);
int simplify_multiplication(Tree* tree);
int simplify_division      (Tree* tree);
int simplify_addition      (Tree* tree);
int simplify_subtraction   (Tree* tree);
int simplify_degree        (Tree* tree);
