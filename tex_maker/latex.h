//
// Created by IvanBrekman on 30.11.2021
//

#ifndef LATEXH
#define LATEXH

#include "../libs/baselib.h"
#include "../libs/tree.h"

#define TYPE node->data.type
#define VAL  node->data.value

struct NodeContext {
    int priority     = -1;
    int nodes_amount = -1;
    char* data = NULL;
};

const int MAX_LATEX_STRING = 300;

int latex_tree(Tree* tree, const char* filename);
int get_priority(node_t data);
int print_context(NodeContext* context);

NodeContext get_node_latex(Node* node);
NodeContext render_template(const char* node_template, NodeContext opp_context, NodeContext lcontext, NodeContext rcontext);

#endif // LATEXH
