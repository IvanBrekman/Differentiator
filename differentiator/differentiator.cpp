//
// Created by IvanBrekman on 21.11.2021
//

#include <cctype>
#include <cstring>

#include "../config.h"
#include "../libs/baselib.h"
#include "../libs/tree.h"

#include "differentiator.h"

#define FUNC(name, val) name,
const char* ALL_FUNCTIONS[] = {
    #include "functions.h"
};
#undef FUNC

int main(int argc, char** argv) {
    if (argc < 2) {
        PRINT_WARNING("Cant parse source_file arg\n");
        return -1;
    }

    printf("Diffirentiator. File path: %s\n", argv[1]);

    Tree tree = { };
    tree_ctor(&tree);

    Node node1 = { };
    node_ctor(&node1, NULL, { data_type::OPP_T, '/' });

    Node node2 = { };
    node_ctor(&node2, NULL, { data_type::OPP_T, '+' });
    Node node3 = { };
    node_ctor(&node3, NULL, { data_type::OPP_T, '-' });

    Node node4 = { };
    node_ctor(&node4, NULL, { data_type::CONST_T, '2' });
    Node node5 = { };
    node_ctor(&node5, NULL, { data_type::VAR_T,   'x' });
    Node node6 = { };
    node_ctor(&node6, NULL, { data_type::CONST_T, '3' });
    Node node7 = { };
    node_ctor(&node7, NULL, { data_type::VAR_T,   'x' });

    tree.root = &node1;
    ADD_CHILD(tree, node1, node2, -1);
    ADD_CHILD(tree, node1, node3,  1);

    ADD_CHILD(tree, node2, node4, -1);
    ADD_CHILD(tree, node2, node5,  1);
    ADD_CHILD(tree, node3, node6, -1);
    ADD_CHILD(tree, node3, node7,  1);

    //Tree_dump(&tree, "Check creating tree");
    //LOG_DUMP_GRAPH(&tree, "Check creating tree", Tree_dump_graph);

    //write_tree_to_file(&tree, "logs/func.txt", write_type::INORDER);

    tree_ctor(&tree);
    read_tree_from_file(&tree, "logs/func.txt");

    Tree_dump(&tree, "check reading tree");
    LOG_DUMP_GRAPH(&tree, "Check creating tree", Tree_dump_graph);

    return 0;
}


int read_tree_from_file(Tree* tree, const char* source_file) {
    ASSERT_IF(VALID_PTR(source_file), "Invalid source_file ptr", 0);

    char* data = get_raw_text(source_file);
    delete_spaces(data);
    LOG1(printf("data: '%s'\n", data););

    LOG1(printf("Start parsing data...\n"););
    int shift = 0;
    Node* root = get_new_node(data, &shift);
    LOG1(printf("End parsing data.\n"););

    tree->root = root;
    update_tree_depth_size(tree);

    ASSERT_OK(tree, Tree, "Check after reading tree", 0);
    return 1;
}


Node* get_new_node(char* data, int* shift) {
    ASSERT_IF(VALID_PTR(data), "Invalid data ptr", (Node*)poisons::UNINITIALIZED_PTR);
    ASSERT_IF(VALID_PTR(shift) || shift == NULL, "Invalid shift ptr", (Node*)poisons::UNINITIALIZED_PTR);

    Node* new_node = (Node*) calloc_s(1, sizeof(Node));
    node_ctor(new_node);

    int i = 0;
    LOG2(get_new_node_func_debug(data, 0, new_node, "Start analyzing node"););

    // Getting terminal node data----------------------------------------------
    if (data[i] != OPEN_BRACKET) {
        LOG2(get_new_node_func_debug(data, i, new_node, "Detected terminal node (or unary function)"););
        int value = -1;
        int node_type = get_node_data(&data[i], &value, shift);

        new_node->data =  { (data_type)node_type, value };
        i += *shift;

        LOG2(get_new_node_func_debug(data, i, new_node, "Check added node"););
        *shift = i + 2;
        return new_node;
    }
    // ------------------------------------------------------------------------

    if (data[i] != OPEN_BRACKET) {
        PRINT_WARNING("Incorrect expression. Cant find left child");
        return NULL;
    }
    GET_LEFT_CHILD(new_node);

    new_node->data = { data_type::OPP_T, data[i++] };

    if (data[i] != OPEN_BRACKET) {
        PRINT_WARNING("Incorrect expression. Cant finc right child");
        return NULL;
    }
    GET_RIGHT_CHILD(new_node);
    
    *shift = i + 2;
    return new_node;
}

int get_node_data(char* data, int* data_store, int* shift) {
    ASSERT_IF(VALID_PTR(data),       "Invalid data ptr",       -1);
    ASSERT_IF(VALID_PTR(data_store), "Invalid data_store ptr", -1);
    ASSERT_IF(VALID_PTR(shift),      "Invalid shift ptr",      -1);

    char* arg_str = (char*) calloc_s(MAX_SPRINTF_STRING_SIZE, sizeof(char));

    int i = 0;
    while (data[i] != CLOSE_BRACKET) {
        arg_str[i] += data[i];
        i++;
    }
    arg_str[i] = '\0';
    *shift = i;

    LOG1(printf( "data:     '%s'\n"
                 "argument: '%s'\n", data, arg_str););

    if (is_number(arg_str)) {
        *data_store = atoi(arg_str);
        return data_type::CONST_T;
    }

    if (arg_str[0] == VARIABLE_SYMBOL && arg_str[1] == '\0') {
        *data_store = VARIABLE_SYMBOL;
        return data_type::VAR_T;
    }

    // for (int i = 0; i < sizeof(functions) / sizeof(functions[0]); i++) {
    //     if (strcmp(arg_str, functions[i])) {
    //         *data_store = 1;
    //     }
    // }

    return -1;
}

int get_new_node_func_debug(const char* data, int index, Node* cur_node, const char* reason) {
    ASSERT_IF(VALID_PTR(data),   "Invalid data ptr",   0);
    ASSERT_IF(VALID_PTR(reason), "Invalid reason ptr", 0);

    printf("Reason: %s\n", reason);

    int len = strlen(data);
    for (int i = 0; i < len; i++) {
        if (i != index) printf("%c", data[i]);
        else            printf(GREEN "%c" NATURAL, data[i]);
    }
    if (index >= len) {
        for (int i = 0; i < index - len; i++) printf(GREEN "_" NATURAL);
        printf(GREEN "↓" NATURAL);
    }

    printf("\ntype: %d; value: '%c'; ptr: %p\n", cur_node->data.type, cur_node->data.value, cur_node);
    printf("\n\n");

    return 1;
}

