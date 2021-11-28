//
// Created by IvanBrekman on 21.11.2021
//

#include <cctype>
#include <cstring>
#include <ctime>
#include <unistd.h>

#include "../config.h"
#include "../libs/baselib.h"
#include "../libs/tree.h"

#include "differentiator.h"

#define FUNC(name, val) { name, val },
const Functions ALL_FUNCTIONS[] = {
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

int print_node_val(Node* node) {
    ASSERT_IF(VALID_PTR(node), "Invalid node ptr", 0);

    switch (node->data.type) {
        case CONST_T:
            printf("%d", node->data.value);
            break;
        case VAR_T:
        case OPP_T:
            printf("%c", node->data.value);
            break;
        case ERROR_T:

        default:
            printf("%c", poisons::UNINITIALIZED_INT);
            break;  
    }

    return 1;
}

const char* get_func_name(int func_code) {
    for (int i = 0; i < sizeof(ALL_FUNCTIONS) / sizeof(ALL_FUNCTIONS[0]); i++) {
        if (ALL_FUNCTIONS[i].code == func_code) {
            return ALL_FUNCTIONS[i].name;
        }
    }

    return NULL;
}

int read_tree_from_file(Tree* tree, const char* source_file) {
    ASSERT_OK(tree, Tree,             "Check tree before reading", 0);
    ASSERT_IF(VALID_PTR(source_file), "Invalid source_file ptr",   0);

    char* data = get_raw_text(source_file);
    delete_spaces(data);
    LOG1(printf("data: '%s'\n", data););

    LOG1(printf("Start parsing data...\n"););
    ParseContext context = { data, 0 };
    Node* root = get_new_node(&context);
    LOG1(printf("End parsing data.\n"););

    tree->root = root;
    update_tree_depth_size(tree);

    ASSERT_OK(tree, Tree, "Check after reading tree", 0);
    return 1;
}


Node* get_new_node(ParseContext* data) {
    ASSERT_IF(VALID_PTR(data), "Invalid data ptr", (Node*)poisons::UNINITIALIZED_PTR);

    Node* new_node = (Node*) calloc_s(1, sizeof(Node));
    node_ctor(new_node);

    LOG2(get_new_node_func_debug(data, new_node, "Start analyzing node"););

    // Getting terminal node data----------------------------------------------
    if (DATA_VAL != OPEN_BRACKET) {
        LOG2(printf("Detected terminal node\n"););
        int value = -1;
        int node_type = get_node_data(data, &value);

        if (node_type == data_type::OPP_T) {
            LOG2(printf("Detected unary function\n"););
            GET_LEFT_CHILD(new_node);
        }

        new_node->data =  { (data_type)node_type, value };

        LOG2(get_new_node_func_debug(data, new_node, "Check added node"););
        INDEX += 1;
        return new_node;
    }
    // ------------------------------------------------------------------------

    if (DATA_VAL != OPEN_BRACKET) {
        PRINT_WARNING("Incorrect expression. Cant find left child");
        return NULL;
    }
    GET_LEFT_CHILD(new_node);

    new_node->data = { data_type::OPP_T, DATA_VAL };
    INDEX++;

    if (DATA_VAL != OPEN_BRACKET) {
        PRINT_WARNING("Incorrect expression. Cant finc right child");
        return NULL;
    }
    GET_RIGHT_CHILD(new_node);
    
    INDEX += 1;
    return new_node;
}

int get_node_data(ParseContext* data, int* data_store) {
    ASSERT_IF(VALID_PTR(data),       "Invalid data ptr",       -1);
    ASSERT_IF(VALID_PTR(data_store), "Invalid data_store ptr", -1);

    char* arg_str = (char*) calloc_s(MAX_SPRINTF_STRING_SIZE, sizeof(char));

    int i = 0, brackets = 1;
    while (brackets != 0) {
        arg_str[i] += DATA[INDEX + i];
        i++;

        if      (DATA[INDEX + i] == CLOSE_BRACKET) brackets--;
        else if (DATA[INDEX + i] ==  OPEN_BRACKET) brackets++;
    }
    arg_str[i] = '\0';

    LOG1(printf( "data:     '%s'\n"
                 "argument: '%s'\n"
                 "index, i: %d, %d\n", DATA, arg_str, INDEX, i););

    if (is_number(arg_str)) {
        INDEX += i;
        *data_store = atoi(arg_str);
        return data_type::CONST_T;
    }

    if (arg_str[0] == VARIABLE_SYMBOL && arg_str[1] == '\0') {
        INDEX += i;
        *data_store = VARIABLE_SYMBOL;
        return data_type::VAR_T;
    }

    *strchr(arg_str, OPEN_BRACKET) = '\0';
    for (int i = 0; i < sizeof(ALL_FUNCTIONS) / sizeof(ALL_FUNCTIONS[0]); i++) {
        if (strcmp(arg_str, ALL_FUNCTIONS[i].name) == 0) {
            INDEX += strlen(arg_str);
            *data_store = ALL_FUNCTIONS[i].code;
            return data_type::OPP_T;
        }
    }

    return -1;
}

int get_new_node_func_debug(const ParseContext* data, Node* cur_node, const char* reason) {
    ASSERT_IF(VALID_PTR(data),     "Invalid data ptr",     0);
    ASSERT_IF(VALID_PTR(cur_node), "Invalid cur_node ptr", 0);
    ASSERT_IF(VALID_PTR(reason),   "Invalid reason ptr",   0);

    printf("Reason: %s\n", reason);
    sleep(0);

    int len = strlen(DATA);
    for (int i = 0; i < len; i++) {
        if (i != INDEX) printf("%c", DATA[i]);
        else            printf(GREEN "%c" NATURAL, DATA[i]);
    }
    if (INDEX >= len) {
        for (int i = 0; i < INDEX - len; i++) printf(GREEN "_" NATURAL);
        printf(GREEN "↓" NATURAL);
    }

    const char* type = NULL;
    switch (cur_node->data.type) {
        case ERROR_T:
            type = "ERROR";
            break;
        case CONST_T:
            type = "CONST";
            break;
        case VAR_T:
            type = "  VAR";
            break;
        case OPP_T:
            type = "  OPP";
            break;

        default:
            type = "UNKNOWN";
            break;  
    }

    printf("\ntype: %s; value: '", type);
    print_node_val(cur_node);
    printf("'; ptr: %p\n", cur_node);
    printf("\n\n");

    return 1;
}

int Tree_dump_graph(Tree* tree, const char* reason, FILE* log, int show_parent_edge) {
    ASSERT_IF(VALID_PTR(tree),   "Invalid tree ptr",   0);
    ASSERT_IF(VALID_PTR(log),    "Invalid log ptr",    0);
    ASSERT_IF(VALID_PTR(reason), "Invalid reason ptr", 0);

    FILE* dot_file = open_file("logs/dot_file.txt", "w");

    fputs("digraph structs {\n", dot_file);
    fputs("\trankdir=HR\n"
          "\tlabel=\"", dot_file);
    fputs(reason, dot_file);
    fputs("\"\n\n", dot_file);

    int size  = tree->size;
    int depth = tree->depth;
    Node* root = tree->root;

    // Fill data to graphiz----------------------------------------------------
    SPR_FPUTS(dot_file, "\tdepth[ shape=component label=\"depth: %d\" ]\n"
                        "\t size[ shape=component label=\"size:  %d\" ]\n",
              depth, size
    );

    fputs("\t{\n\t\tnode[ style=invis ]\n\t\tedge[ style=invis ]\n", dot_file);
    for (int i = 0; i <= depth; i++) {
        SPR_FPUTS(dot_file, "\t\t%d -> %d\n", i - 1, i);
    }

    fputs("\t}\n\n\t{\n\t\trank = same; -1;\n\t\tnode[ style=invis ]\n\t\tedge[ style=invis ]\n", dot_file);
    for (int i = 1; i < size; i++) {
        SPR_FPUTS(dot_file, "\t\thor_%d -> hor_%d\n", i, i + 1);
    }
    fputs("\t}\n", dot_file);

    std::list<Node*> nodes = { };
    get_inorder_nodes(root, &nodes);

    fputs("\n\t{\n\t\tedge[ style=invis weight=1000 ]\n", dot_file);
    int hor_cell_index = 1;
    for (Node* cur_node : nodes) {
        SPR_FPUTS(dot_file, "\t\t%d -> hor_%d\n", INT_ADDRESS(cur_node), hor_cell_index++);
    }
    fputs("\t}\n\n", dot_file);

    for (Node* cur_node : nodes) {
        int err   = Node_error(cur_node);
        int type  = cur_node->data.type;
        int value = cur_node->data.value;

        char* color = (char*)"black";
        if (type == data_type::ERROR_T) color = (char*)"red";
        if (type == data_type::CONST_T) color = (char*)"green";
        if (type == data_type::VAR_T)   color = (char*)"maroon";
        if (type == data_type::OPP_T)   color = (char*)"magenta2";

        char* shape = (char*)"record";
        if (cur_node->data.type == data_type::OPP_T) {
            if (value == opp_type::PLUS)     shape = (char*)"hexagon";
            if (value == opp_type::MINUS)    shape = (char*)"ellipse";
            if (value == opp_type::MULTIPLY) shape = (char*)"diamond";
            if (value == opp_type::DIVISION) shape = (char*)"triangle";
        }

        /* First if checks if shape == "record" Ну да костыдек небольшой */
        if (shape[0] == 'r' && type == data_type::OPP_T)   SPR_FPUTS(dot_file, "\t%d[ shape=%s label=\"%s\" width=2 fontsize=25 color=\"%s\" ]\n", INT_ADDRESS(cur_node), shape, get_func_name(cur_node->data.value), color);
        else if               (type != data_type::CONST_T) SPR_FPUTS(dot_file, "\t%d[ shape=%s label=\"%c\" width=2 fontsize=25 color=\"%s\" ]\n", INT_ADDRESS(cur_node), shape, cur_node->data.value, color);
        else                                               SPR_FPUTS(dot_file, "\t%d[ shape=%s label=\"%d\" width=2 fontsize=25 color=\"%s\" ]\n", INT_ADDRESS(cur_node), shape, cur_node->data.value, color);

        if (cur_node->parent != NULL && show_parent_edge) {
            SPR_FPUTS(dot_file, "\t%d -> %d\n", INT_ADDRESS(cur_node), INT_ADDRESS(cur_node->parent));
        }
        if (cur_node->left   != NULL) {
            SPR_FPUTS(dot_file, "\t%d -> %d\n[ label=\"L\" ]", INT_ADDRESS(cur_node), INT_ADDRESS(cur_node->left));
        }
        if (cur_node->right  != NULL) {
            SPR_FPUTS(dot_file, "\t%d -> %d\n[ label=\"R\" ]", INT_ADDRESS(cur_node), INT_ADDRESS(cur_node->right));
        }
        fputs("\n", dot_file);
    }

    nodes.clear();
    nodes.push_back(root);

    int cur_depth = -1;
    while (!nodes.empty()) {
        Node* pop_node = nodes.front();
        nodes.pop_front();

        if (pop_node->left  != NULL) nodes.push_back(pop_node->left);
        if (pop_node->right != NULL) nodes.push_back(pop_node->right);

        if (pop_node->depth != cur_depth) {
            cur_depth = pop_node->depth;
            SPR_FPUTS(dot_file, "%s\t{ rank = same; %d; %d;", pop_node == root ? "" : "}\n", cur_depth, INT_ADDRESS(pop_node));
        } else {
            SPR_FPUTS(dot_file, " %d;", INT_ADDRESS(pop_node));
        }
    }
    fputs(" }\n", dot_file);
    

    nodes.~list();
    // ------------------------------------------------------------------------

    fputs("}\n", dot_file);
    fclose(dot_file);

    time_t seconds = time(NULL);
    SPR_SYSTEM("dot -Tpng logs/dot_file.txt -o logs/graph_%ld.png", seconds);

    fputs("<h1 align=\"center\">Dump Tree</h1>\n<pre>\n", log);
    Tree_dump(tree, reason, log);
    SPR_FPUTS(log, "</pre>\n<img src=\"logs/graph_%ld.png\">\n\n", seconds);

    return seconds;
}
