//
// Created by IvanBrekman on 21.11.2021
//

#include "../config.h"

#include "../libs/baselib.h"
#include "../libs/tree.h"

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
