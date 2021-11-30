//
// Created by IvanBrekman on 29.11.2021
//

#include <list>
#include <cmath>

#include "../config.h"
#include "../libs/baselib.h"

#include "simplifier.h"
#include "../diff/differentiator.h"
#include "../diff/DSL.h"

#undef NEW_VCONST
#undef NEW_OPP
#define NEW_VCONST(val)    create_new_node(data_type::CONST_T, val, NULL, NULL, NULL)
#define NEW_OPP(val, L, R) create_new_node(data_type::OPP_T,   val, cur_node->parent, L, R)

int simplify(Tree* tree) {
    ASSERT_OK(tree, Tree, "Check before simplify func", -1);

    int simplifications_amount = 0;
    int cur_simp = 0;
    while (1) {
        cur_simp += simplify_constants     (tree);
        cur_simp += simplify_multiplication(tree);
        cur_simp += simplify_division      (tree);
        cur_simp += simplify_addition      (tree);
        cur_simp += simplify_subtraction   (tree);
        cur_simp += simplify_degree        (tree);
        update_tree_depth_size(tree);

        if      (cur_simp == simplifications_amount) break;
        else if (cur_simp <  simplifications_amount) return -1;

        simplifications_amount = cur_simp;
        Tree_dump(tree, "Check simp iteration");
        LOG_DUMP_GRAPH(tree, "Check simp iteration", Tree_dump_graph);
        getchar();
    }

    update_tree_depth_size(tree);

    ASSERT_OK(tree, Tree, "Check after simplify func", -1);
    return simplifications_amount;
}

int simplify_constants(Tree* tree) {
    ASSERT_OK(tree, Tree, "Check before simplify_constants func", -1);

    std::list<Node*> nodes = { tree->root };

    int s_amount = 0;
    while (!nodes.empty()) {
        Node* cur_node = nodes.front();
        nodes.pop_front();

        errno = 0;
        double res = calc_node(cur_node);

        if (errno == 0 && IS_CONST(res) && is_integer(res) && cur_node->data.type != data_type::CONST_T) {
            Node* node = NEW_VCONST((int)res);
            printf("parent: %p; ptr: %p; res: %lf\n", cur_node->parent, node, res);
            UPDATE_DATA;
            printf("node parent: %p\n", node->parent);
        } else {
            ADD_CHILDRENS(nodes, cur_node);
        }
    }

    ASSERT_OK(tree, Tree, "Check after simplify_constants func", -1);
    return s_amount;
}

int simplify_multiplication(Tree* tree) {
    ASSERT_OK(tree, Tree, "Check before simplify_multiplication func", -1);

    std::list<Node*> nodes = { tree->root };

    int s_amount = 0;
    while (!nodes.empty()) {
        Node* cur_node = nodes.front();
        nodes.pop_front();

        if (CHECK_OPP(opp_type::MULTIPLY)) {
            if        (LCONST(1) || RCONST(1)) {
                Node* node = copy_node(LCONST(1) ? cur_node->right : cur_node->left, cur_node->parent);
                UPDATE_DATA;
            } else if (LCONST(0) || RCONST(0)) {
                Node* node = NEW_VCONST(0);
                UPDATE_DATA;
            }
        } 
        ADD_CHILDRENS(nodes, cur_node);
    }
    
    ASSERT_OK(tree, Tree, "Check after simplify_multiplication func", -1);
    return s_amount;
}

int simplify_division(Tree* tree) {
    ASSERT_OK(tree, Tree, "Check before simplify_division func", -1);

    std::list<Node*> nodes = { tree->root };

    int s_amount = 0;
    while (!nodes.empty()) {
        Node* cur_node = nodes.front();
        nodes.pop_front();

        if (CHECK_OPP(opp_type::DIVISION)) {
            if        (RCONST(1)) {
                Node* node = copy_node(cur_node->left, cur_node->parent);
                UPDATE_DATA;
            } else if (RCONST(0)) {
                PRINT_WARNING("Detected division by zero");
            } else if (LCONST(0)) {
                Node* node = NEW_VCONST(0);
                UPDATE_DATA;
            }
        } 
        ADD_CHILDRENS(nodes, cur_node);
    }

    ASSERT_OK(tree, Tree, "Check after simplify_division func", -1);
    return s_amount;
}

int simplify_addition(Tree* tree) {
    ASSERT_OK(tree, Tree, "Check before simplify_addition func", -1);

    std::list<Node*> nodes = { tree->root };

    int s_amount = 0;
    while (!nodes.empty()) {
        Node* cur_node = nodes.front();
        nodes.pop_front();

        if (CHECK_OPP(opp_type::PLUS)) {
            if (LCONST(0) || RCONST(0)) {
                Node* node = copy_node(LCONST(1) ? cur_node->right : cur_node->left, cur_node->parent);
                UPDATE_DATA;
            }
        } 
        ADD_CHILDRENS(nodes, cur_node);
    }
    
    ASSERT_OK(tree, Tree, "Check after simplify_addition func", -1);
    return s_amount;
}

int simplify_subtraction(Tree* tree) {
    ASSERT_OK(tree, Tree, "Check before simplify_substraction func", -1);

    std::list<Node*> nodes = { tree->root };

    int s_amount = 0;
    while (!nodes.empty()) {
        Node* cur_node = nodes.front();
        nodes.pop_front();

        if (CHECK_OPP(opp_type::MINUS)) {
            if        (RCONST(0)) {
                Node* node = copy_node(cur_node->left, cur_node->parent);
                UPDATE_DATA;
            } else if (LCONST(0)) {
                Node* node = NEW_OPP(opp_type::MULTIPLY, NEW_VCONST(-1), copy_node(cur_node->right, cur_node->parent));
                UPDATE_DATA;
            }
        }
        ADD_CHILDRENS(nodes, cur_node);
    }
    
    ASSERT_OK(tree, Tree, "Check after simplify_substraction func", -1);
    return s_amount;
}

int simplify_degree(Tree* tree) {
    ASSERT_OK(tree, Tree, "Check before simplify_degree func", -1);

    std::list<Node*> nodes = { tree->root };

    int s_amount = 0;
    while (!nodes.empty()) {
        Node* cur_node = nodes.front();
        nodes.pop_front();

        if (CHECK_OPP(opp_type::DEGREE)) {
            if        (LCONST(0) || LCONST(1)) {
                Node* node = NEW_VCONST(LCONST(0) ? 0 : 1);
                UPDATE_DATA;
            } else if (RCONST(0)) {
                Node* node = NEW_VCONST(1);
                UPDATE_DATA;
            } else if (RCONST(1)) {
                Node* node = copy_node(cur_node->left, cur_node->parent);
                UPDATE_DATA;
            }
        }
        ADD_CHILDRENS(nodes, cur_node);
    }
    
    ASSERT_OK(tree, Tree, "Check after simplify_degree func", -1);
    return s_amount;
}

#undef NEW_VCONST
#undef NEW_OPP
