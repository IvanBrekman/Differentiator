//
// Created by IvanBrekman on 30.11.2021
//

#include <cstring>

#include "../config.h"
#include "../diff/differentiator.h"
#include "../diff/DSL.h"

#include "latex.h"

#define FUNC(name, priority, template, code) { name, template, get_code(name), priority },
const Functions ALL_FUNCTIONS[] = {
    #include "../diff/functions.h"
    FUNC( "ln", 5, "//ln{L}", DIV(DL, CL) )
};
#undef FUNC

int latex_tree(Tree* tree, const char* filename) {
    ASSERT_OK(tree, Tree,  "Check before latex_tree func", 0);
    ASSERT_IF(VALID_PTR(filename), "Invalid filename ptr", 0);

    NodeContext tree_context = get_node_latex(tree->root);
    ASSERT_IF(VALID_PTR(tree_context.data), "Error in get_node_latex func. Invalid data ptr", 0);

    LOG1(printf("result latex: '%s'\n", tree_context.data););
    WAIT_INPUT;

    FILE* tex_file = open_file(filename, "w");

    close_file(tex_file);

    FREE_PTR(tree_context.data, char);
    return 1;
}

int get_priority(node_t data) {
    switch (data.type) {
        case data_type::ERROR_T:
            return poisons::UNINITIALIZED_INT;
        case data_type::CONST_T:
        case data_type::VAR_T:
            return 4;
        case data_type::OPP_T:
            switch (data.value) {
                case opp_type::PLUS:
                case opp_type::MINUS:
                    return 1;
                case opp_type::MULTIPLY:
                case opp_type::DIVISION:
                    return 2;
                case::DEGREE:
                    return 3;
                
                default:
                    for (int i = 0; i < (int)(sizeof(ALL_FUNCTIONS) / sizeof(ALL_FUNCTIONS[0])); i++) {
                        if (ALL_FUNCTIONS[i].code == data.value) {
                            return ALL_FUNCTIONS[i].priority;
                        }
                    }

                    return poisons::UNINITIALIZED_INT;
            }

        default:
            return poisons::UNINITIALIZED_INT;
    }
}

int print_context(NodeContext* context) {
    printf( "data:      '%s'\n"
            "priority:  %d\n"
            "nd_amount: %d\n\n", context->data, context->priority, context->nodes_amount
    );

    return 1;
}

NodeContext get_node_latex(Node* node) {
    ASSERT_OK(node, Node, "Check before get_node_latex func", { });

    int cur_priority = get_priority(node->data);
    ASSERT_IF(cur_priority != poisons::UNINITIALIZED_INT, "Cant get node priority", { });
    
    if (TYPE == data_type::CONST_T) {
        NodeContext context = { cur_priority, 1, (char*) to_string(VAL) };
        return context;
    }
    if (TYPE == data_type::VAR_T) {
        char* str = (char*) calloc_s(2, sizeof(char));
        str[0] = VARIABLE_SYMBOL;
        str[1] = '\0';

        NodeContext context = { cur_priority, 1, str };
        return context;
    }

    NodeContext  context = { cur_priority };
    NodeContext lcontext = { };
    NodeContext rcontext = { };

    if (VALID_PTR(node->left)) lcontext = get_node_latex(node->left);
    if (VALID_PTR(node->left)) rcontext = get_node_latex(node->right);

    LOG2(print_context(&lcontext););
    LOG2(print_context(&rcontext););

    switch (VAL) {
        case opp_type::PLUS:
            return render_template("L + R",      context, lcontext, rcontext);
        case opp_type::MINUS:
            return render_template("L - R",      context, lcontext, rcontext);
        case opp_type::MULTIPLY:
            return render_template("L \\cdot R", context, lcontext, rcontext);
        case opp_type::DIVISION:
            return render_template("\\frac{L}{R}",   context, lcontext, rcontext);
        case opp_type::DEGREE:
            return render_template("{L}^{R}",        context, lcontext, rcontext);
        
        default:
            return { };
    }
}

NodeContext render_template(const char* node_template, NodeContext opp_context, NodeContext lcontext, NodeContext rcontext) {
    ASSERT_IF(VALID_PTR(node_template), "Invalid node_template ptr", { });
    ASSERT_IF(strlen(node_template) + strlen(lcontext.data) + strlen(rcontext.data) + 4 < MAX_LATEX_STRING,
              "Too big latex string. Increase MAX_LATEX_STRING constant", { });

    char* node_string = (char*) calloc_s(MAX_LATEX_STRING, sizeof(char));

    int str_index = 0;
    for (int i = 0; node_template[i] != '\0'; i++) {
        if (node_template[i] == 'L' || node_template[i] == 'R') {
            NodeContext cur_context = node_template[i] == 'L' ? lcontext : rcontext;

            if (opp_context.priority > cur_context.priority) node_string[str_index++] = '(';

            node_string[str_index] = '\0';
            str_index += (int) strlen(cur_context.data);
            LOG2(printf("cutting...\n"
                        "node_string: '%s'\n"
                        "data:        '%s'\n\n", node_string, cur_context.data););
            strcat(node_string, cur_context.data);

            if (opp_context.priority > cur_context.priority) node_string[str_index++] = ')';

            continue;
        }

        node_string[str_index++] = node_template[i];
    }
    node_string[str_index] = '\0';

    LOG1(printf("template: %s\n"
                "render:   %s\n\n", node_template, node_string);
    );

    // dbg(printf("lcontext data: %p (%d): '%s'\nrcontext data: %p (%d): '%s'\n\n", lcontext.data, VALID_PTR(lcontext.data), lcontext.data, rcontext.data, VALID_PTR(rcontext.data), rcontext.data););
    // FREE_PTR(lcontext.data, char);
    FREE_PTR(rcontext.data, char);
    return { opp_context.priority, lcontext.nodes_amount + rcontext.nodes_amount, node_string };
}
