//
// Created by IvanBrekman on 28.11.2021
//

#define NEW_VCONST(val)    create_new_node(data_type::CONST_T, val, node->parent, NULL, NULL)
#define NEW_OPP(val, L, R) create_new_node(data_type::OPP_T,   val, node->parent, L, R)

#define NEW_CONST NEW_VCONST(0)
#define   NEW_VAL NEW_VCONST(1)

#define DL get_D(node->left,  session)
#define DR get_D(node->right, session)
#define CL copy_node(node->left,  node->parent)
#define CR copy_node(node->right, node->parent)

#define ADD(L, R) NEW_OPP(opp_type::PLUS,     L, R)
#define SUB(L, R) NEW_OPP(opp_type::MINUS,    L, R)
#define MUL(L, R) NEW_OPP(opp_type::MULTIPLY, L, R)
#define DIV(L, R) NEW_OPP(opp_type::DIVISION, L, R)
#define DEG(L, R) NEW_OPP(opp_type::DEGREE,   L, R)
