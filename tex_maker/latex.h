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

FILE* latex_init_session(const char* filename);
int   latex_end_session(FILE* tex_file);
int   latex_to_pdf(const char* latex_file);

int latex_tree(Tree* tree, FILE* session);
int get_priority(node_t data);
int print_context(NodeContext* context);

NodeContext get_node_latex(Node* node);
NodeContext render_template(const char* node_template, NodeContext opp_context, NodeContext lcontext, NodeContext rcontext);

#endif // LATEXH
