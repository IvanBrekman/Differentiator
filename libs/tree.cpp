//
// Created by IvanBrekman on 09.11.2021.
//

#include "../config.h"

#include <cassert>
#include <cerrno>
#include <ctime>
#include <cstring>

#include "baselib.h"

#include "tree.h"

int tree_ctor(Tree* tree) {
    ASSERT_IF(VALID_PTR(tree), "Invalid tree ptr", 0);

    Node root = { };
    node_ctor(&root, NULL, (node_t)INIT_VALUE);

    tree->root  = &root;
    tree->size  = 1;
    tree->depth = 0;

    ASSERT_OK(tree, Tree, "Check after ctor call", 0);
    return 1;
}

int tree_dtor(Tree* tree) {
    ASSERT_OK(tree, Tree, "Check before dtor call", 0);

    node_dtor(tree->root);
    tree->size  = -1;
    tree->depth = -1;

    return 1;
}

int node_ctor(Node* node, Node* parent, node_t value) {
    ASSERT_IF(VALID_PTR(node),    "Invalid node ptr",   0);
    ASSERT_IF(VALID_NODE(parent), "Invalid parent ptr", 0);

    node->left_canary  = CANARY;
    node->right_canary = CANARY;

    node->data = value;

    node->parent = parent;
    node->left   = NULL;
    node->right  = NULL;

    node->depth  = 0;

    ASSERT_OK(node, Node, "Check after ctor call", 0);
    return 1;
}

int node_dtor(Node* node) {
    ASSERT_OK(node, Node, "Check before dtor call", 0);

    if (node->left  != NULL) node_dtor(node->left);
    if (node->right != NULL) node_dtor(node->right);

    node->data   = DEINIT_VALUE;
    node->parent = (Node*)poisons::FREED_PTR;
    node->left   = (Node*)poisons::FREED_PTR;
    node->right  = (Node*)poisons::FREED_PTR;

    node->depth  = poisons::FREED_ELEMENT;

    node->left_canary  = POISON_CANARY;
    node->right_canary = POISON_CANARY;

    return 1;
}

const char* error_desc(int error_code) {
    switch (error_code)
    {
    case errors::OK_:
        return "ok";
    
    case errors::INVALID_NODE_PTR:
        return "Invalid ptr to Node object";
    case errors::DAMAGED_LCANARY:
        return "Incorrect value of left node canary. May be it isn`t ptr to Node object";
    case errors::DAMAGED_RCANARY:
        return "Incorrect value of right node canary. May be it isn`t ptr to Node object";
    case errors::POISONED_LCANARY:
        return "POISONED value of left node canary. Its ptr to killed Node!!!";
    case errors::POISONED_RCANARY:
        return "POISONED value of left node canary. Its ptr to killed Node!!!";
    case errors::INVALID_PARENT:
        return "Invalid ptr to Node->parent object";
    case errors::INVALID_LEFT:
        return "Invalid ptr to Node->left child object";
    case errors::INVALID_RIGHT:
        return "Invalid ptr to Node->right child object";
    
    case errors::INVALID_TREE_PTR:
        return "Invalid ptr to Tree object";
    case errors::INCORRECT_SIZE:
        return "Incorrect tree size value. Should be (>= 0)";
    case errors::INCORRECT_DEPTH:
        return "Incorrect depth value. Should be (>= 0)";
    
    default:
         return "Unknown value";
    }
}

int Tree_error(Tree* tree) {
    if (!VALID_PTR(tree)) {
        return errors::INVALID_TREE_PTR;
    }

    if (tree->size < 0) {
        return errors::INCORRECT_SIZE;
    }
    if (tree->depth < 0) {
        return errors::INCORRECT_DEPTH;
    }

    int err = Node_error(tree->root, 1);
    if (err) return err;

    return errors::OK_;
}

int Node_error(Node* node, int recursive_check, int (*node_data_ok)(node_t data)) {
    if (!VALID_PTR(node)) {
        return errors::INVALID_NODE_PTR;
    }

    if (node->left_canary  != CANARY) {
        return node->left_canary  == POISON_CANARY ? errors::POISONED_LCANARY : errors::DAMAGED_LCANARY;
    }
    if (node->right_canary != CANARY) {
        return node->right_canary == POISON_CANARY ? errors::POISONED_RCANARY : errors::DAMAGED_RCANARY;
    }

    if (!VALID_NODE(node->parent)) return errors::INVALID_PARENT;
    if (!VALID_NODE(node->left))   return errors::INVALID_LEFT;
    if (!VALID_NODE(node->right))  return errors::INVALID_RIGHT;

    if (node->depth < 0) {
        return errors::INCORRECT_DEPTH;
    }

    if (VALID_PTR(node_data_ok) && !node_data_ok(node->data)) {
        return errors::INCORRECT_DATA;
    }

    int err = -1;
    if (node->left  != NULL && recursive_check) {
        err = Node_error(node->left,  1);
        if (err) return err;
    }
    if (node->right != NULL && recursive_check) {
        err = Node_error(node->right, 1);
        if (err) return err;
    }

    return errors::OK_;
}

int add_child(Node* parent, Node* child, int is_left_child) {
    ASSERT_OK(parent, Node, "Check parent before add_child func", 0);
    ASSERT_OK(child,  Node, "Check child  before add_child func", 0);
    ASSERT_IF(is_left_child == -1 || is_left_child == 1, "Incorecct is_left_child value. Should be -1 or 1", 0);

    if (is_left_child == -1) parent->left  = child;
    else                     parent->right = child;

    child->parent = parent;
    child->depth  = parent->depth + 1;

    return child->depth;
}

int is_leaf(Node* node) {
    ASSERT_OK(node, Node, "Check before is_leaf func", 0);

    return node->left == NULL && node->right == NULL;
}

int is_full_node(Node* node) {
    ASSERT_OK(node, Node, "Check before is_full_node func", 0);

    return node->left != NULL && node->right != NULL;
}

int is_left_child(Node* node) {
    ASSERT_OK(node, Node, "Check before is_left_child func", 0);

    return node->parent != NULL && node->parent->left == node;
}

int is_right_child(Node* node) {
    ASSERT_OK(node, Node, "Check before is_right_child func", 0);

    return node->parent != NULL && node->parent->right == node;
}

Node* find_node_by_value(Tree* tree, node_t value, std::list<NodeDesc>* path, int (*value_comparator) (node_t val1, node_t val2)) {
    ASSERT_OK(tree, Tree, "Invalid tree ptr", NULL);
    ASSERT_IF(VALID_PTR(path), "Invalid path ptr", NULL);

    std::list<Node*> nodes = { };
    get_inorder_nodes(tree->root, &nodes);

    Node* need_node = NULL;
    for (Node* cur_node : nodes) {
        if (value_comparator(cur_node->data, value) == 0) need_node = cur_node;
    }

    if (need_node == NULL) return NULL;

    Node* cur_node = need_node;
    Node* prev_node = NULL;
    while (cur_node != NULL) {
        path->push_front({ cur_node, cur_node->left == prev_node });

        prev_node = cur_node;
        cur_node  = cur_node->parent;
    }

    return need_node;
}

int get_inorder_nodes(Node* node, std::list<Node*>* nodes) {
    ASSERT_OK(node, Node, "Check before get_inorder func", 0);
    ASSERT_IF(VALID_PTR(nodes), "Invalid nodes ptr", 0);

    if (node->left  != NULL) get_inorder_nodes(node->left,  nodes);
    nodes->push_back(node);
    if (node->right != NULL) get_inorder_nodes(node->right, nodes);

    return 1;
}

int Node_dump(Node* node, const char* reason, FILE* log) {
    ASSERT_IF(VALID_PTR(node),   "Invalid node ptr",   0);
    ASSERT_IF(VALID_PTR(reason), "Invalid reason ptr", 0);
    ASSERT_IF(VALID_PTR(log),    "Invalid log ptr",    0);

    fprintf(log, "Node <");
    fprintf(log, COLORED_OUTPUT("%p", PURPLE, log), node);
    fprintf(log, ">----------------------------------------------------------\n");
    FPRINT_DATE(log);
    fprintf(log, "\n");

    fprintf(log, COLORED_OUTPUT("%s\n", BLUE, log), reason);
    int err = Node_error(node);

    fprintf(log, " Node state:    %d ", err);
    if (err != 0) fprintf(log, COLORED_OUTPUT("(%s)\n", RED,   log), error_desc(err));
    else          fprintf(log, COLORED_OUTPUT("(%s)\n", GREEN, log), error_desc(err));

    fprintf(log, " Depth in tree: %d %s\n\n", node->depth, node->depth < 0 ? COLORED_OUTPUT("(BAD)", RED, log) : "");

    fprintf(log, "  Left canary: %lX %s\n"
                 " Right canary: %lX %s\n\n",
            node->left_canary,  node->left_canary  == CANARY ? "" : node->left_canary  == POISON_CANARY ? COLORED_OUTPUT("(POISON)", PURPLE, log) : COLORED_OUTPUT("(BAD)", RED, log),
            node->right_canary, node->right_canary == CANARY ? "" : node->right_canary == POISON_CANARY ? COLORED_OUTPUT("(POISON)", PURPLE, log) : COLORED_OUTPUT("(BAD)", RED, log)
    );

    fprintf(log, " Parent: %p %s\n"
                 " Left:   %p %s\n"
                 " Right:  %p %s\n\n",
            node->parent, VALID_PTR(node->parent) ? "" : node->parent == NULL ? COLORED_OUTPUT("NULL", BLUE, log) : COLORED_OUTPUT("(BAD)", RED, log),
            node->left,   VALID_PTR(node->left)   ? "" : node->left   == NULL ? COLORED_OUTPUT("NULL", BLUE, log) : COLORED_OUTPUT("(BAD)", RED, log),
            node->right,  VALID_PTR(node->right)  ? "" : node->right  == NULL ? COLORED_OUTPUT("NULL", BLUE, log) : COLORED_OUTPUT("(BAD)", RED, log)
    );

    fprintf(log, " Data: type - %2d\n"
                 "      value - '%c'\n", node->data.type, node->data.value);
    fprintf(log, "-------------------------------------------------------------------------------\n");

    return 1;
}

int Tree_dump(Tree* tree, const char* reason, FILE* log) {
    ASSERT_IF(VALID_PTR(tree),   "Invalid tree ptr",   0);
    ASSERT_IF(VALID_PTR(reason), "Invalid reason ptr", 0);
    ASSERT_IF(VALID_PTR(log),    "Invalid log ptr",    0);

    fprintf(log, COLORED_OUTPUT("|-------------------------          Tree  Dump          -------------------------|\n", ORANGE, log));
    FPRINT_DATE(log);
    fprintf(log, COLORED_OUTPUT("%s\n", BLUE, log), reason);
    int err = Tree_error(tree);

    fprintf(log, "    Tree state: %d ", err);
    if (err != 0) fprintf(log, COLORED_OUTPUT("(%s)\n\n", RED,   log), error_desc(err));
    else          fprintf(log, COLORED_OUTPUT("(%s)\n\n", GREEN, log), error_desc(err));

    fprintf(log, "Tree size:  %d %s\n"
                 "Tree depth: %d %s\n\n",
            tree->size,  tree->size  < 0 ? COLORED_OUTPUT("(BAD)", RED, log) : "",
            tree->depth, tree->depth < 0 ? COLORED_OUTPUT("(BAD)", RED, log) : ""
    );

    std::list<Node*> nodes = { };

    nodes.push_back(tree->root);
    while (!nodes.empty()) {
        Node* pop_node = nodes.front();
        nodes.pop_front();

        if (pop_node->left  != NULL) nodes.push_back(pop_node->left);
        if (pop_node->right != NULL) nodes.push_back(pop_node->right);

        int exit_code = Node_dump(pop_node, reason, log);
        if (exit_code != 1) fprintf(log, COLORED_OUTPUT("Node_dump bad exit code (%d)", RED, log), exit_code);
    }
    nodes.~list();

    fprintf(log, COLORED_OUTPUT("|---------------------Compilation  Date %s %s---------------------|", ORANGE, log),
            __DATE__, __TIME__);
    fprintf(log, "\n\n");

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
        int err = Node_error(cur_node);

        char* color = (char*)"black";
        if (cur_node->data.type == data_type::ERROR_T) color = (char*)"red";
        if (cur_node->data.type == data_type::CONST_T) color = (char*)"green";
        if (cur_node->data.type == data_type::VAR_T)   color = (char*)"maroon";
        if (cur_node->data.type == data_type::OPP_T)   color = (char*)"magenta2";

        char* shape = (char*)"record";
        if (cur_node->data.type == data_type::OPP_T) {
            if (cur_node->data.value == opp_type::PLUS)     shape = (char*)"hexagon";
            if (cur_node->data.value == opp_type::MINUS)    shape = (char*)"ellipse";
            if (cur_node->data.value == opp_type::MULTIPLY) shape = (char*)"diamond";
            if (cur_node->data.value == opp_type::DIVISION) shape = (char*)"triangle";
        }

        SPR_FPUTS(dot_file, "\t%d[ shape=%s label=\"%c\" width=2 fontsize=25 color=\"%s\" ]\n", INT_ADDRESS(cur_node), shape, cur_node->data.value, color);

        if (cur_node->parent != NULL && show_parent_edge) {
            SPR_FPUTS(dot_file, "\t%d -> %d\n", INT_ADDRESS(cur_node), INT_ADDRESS(cur_node->parent));
        }
        if (cur_node->left   != NULL) {
            SPR_FPUTS(dot_file, "\t%d -> %d\n", INT_ADDRESS(cur_node), INT_ADDRESS(cur_node->left));
        }
        if (cur_node->right  != NULL) {
            SPR_FPUTS(dot_file, "\t%d -> %d\n", INT_ADDRESS(cur_node), INT_ADDRESS(cur_node->right));
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

int write_tree_to_file(Tree* tree, const char* filename) {
    ASSERT_OK(tree, Tree, "Check before write_tree_to_file func", 0);
    ASSERT_IF(VALID_PTR(filename), "Invalid filename ptr", 0);

    FILE* file = open_file(filename, "w");

    preorder_write_nodes_to_file(tree->root, file);
    fputs("\n", file);
    
    fclose(file);
    return 1;
}

int preorder_write_nodes_to_file(Node* node, FILE* file) {
    ASSERT_OK(node, Node, "Invalid node ptr", 0);
    ASSERT_IF(VALID_PTR(file), "Invalid file ptr", 0);

    char* node_str = (char*) calloc(MAX_SPRINTF_STRING_SIZE, sizeof(char));

    sprintf(node_str, "{ \"%c\" ", node->data.value);
    fputs(node_str, file);

    if (node->left  != NULL) preorder_write_nodes_to_file(node->left,  file);
    else fputs("n ", file);

    if (node->right != NULL) preorder_write_nodes_to_file(node->right, file);
    else fputs("n ", file);

    fputs("} ", file);

    FREE_PTR(node_str, char);
    return 1;
}

int read_tree_from_file(Tree* tree, const char* source_file) {
    ASSERT_IF(VALID_PTR(source_file), "Invalid source_file ptr", 0);

    char* data = get_raw_text(source_file);

    LOG1(printf("Start parsing data...\n"););
    std::list<Node_Child> added_nodes = { };
    int shift = 0;
    int analyze_result = analyze_tree_data(data, tree, &added_nodes, &shift);
    LOG1(printf("End parsing data.\n"););

    ASSERT_OK(tree, Tree, "Check after reading tree", 0);
    return analyze_result;
}

int update_tree_depth_size(Tree* tree) {
    ASSERT_OK(tree, Tree, "Chek before update_tree_depth_size func", 0);

    int size = 0;
    std::list<Node*> nodes = { tree->root };
    while (!nodes.empty()) {
        Node* cur_node = nodes.front();
        
        cur_node->depth = cur_node->parent != NULL ? cur_node->parent->depth + 1 : 0;
        tree->depth = cur_node->depth;
        nodes.pop_front();

        if (cur_node->left  != NULL) nodes.push_back(cur_node->left);
        if (cur_node->right != NULL) nodes.push_back(cur_node->right);

        size++;
    }
    tree->size = size;

    ASSERT_OK(tree, Tree, "Chek after update_tree_depth_size func", 0);
    return 1;
}

int analyze_tree_data(char* data, Tree* tree, std::list<Node_Child>* added_nodes, int* shift) {
    ASSERT_IF(VALID_PTR(data), "Invalid data ptr", 999);
    ASSERT_IF(VALID_PTR(tree), "Invalid tree ptr", 999);
    ASSERT_IF(VALID_PTR(shift) || shift == NULL, "Invalid shift ptr", 999);

    int i = IND_STRCHR(data, 0, '{');
    int start_pos = i;

    if (i < 0) return -1;           // '{' hasnt found
    LOG2(analyze_func_debug(data, i, added_nodes, "Check state at start"););

    // Find indexes of open and close quoter-------------------------------
    i += 2;
    if (data[i] != '"') return i;   // Open  quote hasnt found
    int close_qt = IND_STRCHR(data, i + 1, '"');
    if (close_qt < 0) return i + 1; // Close quote hasnt found
    LOG2(analyze_func_debug(data, i, added_nodes, "Find close quote"););
    // --------------------------------------------------------------------

    // Create new node with data between " "-------------------------------
    data[close_qt] = '\0';

    Node* new_node = (Node*) calloc(1, sizeof(Node));
    //node_ctor(new_node, NULL, (node_t)strdup(&data[i + 1]));
    added_nodes->push_back({ new_node, 0 });

    data[close_qt] = '"';
    LOG2(analyze_func_debug(data, i, added_nodes, "Check added node"););
    // --------------------------------------------------------------------

    // Parse first child---------------------------------------------------
    i = close_qt + 2;
    LOG2(analyze_func_debug(data, i, added_nodes, "Checks first child"););
    if (data[i] == 'n') {
        if (added_nodes->empty()) return i; // List of nodes is empty (expected some element)

        Node_Child tmp = added_nodes->back();
        tmp.child_amount++;

        added_nodes->pop_back();
        added_nodes->push_back(tmp);
    } else if (data[i] == '{') {
        LOG2(analyze_func_debug(data, i, added_nodes, "Call recursive for new node at first child"););

        int anal_res = analyze_tree_data(&data[i], tree, added_nodes, shift);  // Analyze new node
        if (anal_res != -1) return anal_res;

        i += *shift;
        LOG2(analyze_func_debug(data, i, added_nodes, "Ended analyzing node at first child"););
    } else {
        return i;                   // Incorrect seq continuation
    }
    LOG2(analyze_func_debug(data, i, added_nodes, "End checks first child"););
    // --------------------------------------------------------------------

    // Parse second child--------------------------------------------------
    i += 2;
    LOG2(analyze_func_debug(data, i, added_nodes, "Checks second child"););
    if (data[i] == 'n') {
        if (added_nodes->empty()) return i; // List of nodes is empty (expected some element)

        Node_Child tmp = added_nodes->back();
        tmp.child_amount++;
        
        added_nodes->pop_back();
        added_nodes->push_back(tmp);
    } else if (data[i] == '{') {
        LOG2(analyze_func_debug(data, i, added_nodes, "Call recursive for new node at first child"););

        int anal_res = analyze_tree_data(&data[i], tree, added_nodes, shift);  // Analyze new node
        if (anal_res != -1) return anal_res;

        i += *shift;
        LOG2(analyze_func_debug(data, i, added_nodes, "Ended analyzing node at second child"););
    } else {
        return i;                   // Incorrect seq continuation
    }
    LOG2(analyze_func_debug(data, i, added_nodes, "End checks second child"););
    // --------------------------------------------------------------------

    // Check correctness node ending-------------------------------------------
    i += 2;
    if (data[i] != '}') return i;   // Incorrect seq continuation
    if (added_nodes->empty()) return i; // List of nodes is empty (expected some element)

    Node_Child ready_node = added_nodes->back();
    added_nodes->pop_back();
    if (ready_node.child_amount != 2) return i; // Incorrect child amount
    // ------------------------------------------------------------------------

    // Adding child to parent node---------------------------------------------
    LOG2(analyze_func_debug(data, i, added_nodes, "Check node ending"););
    if (added_nodes->empty()) {     // It is root of tree
        tree->root = ready_node.node;
        update_tree_depth_size(tree);

        ASSERT_OK(tree, Tree, "Check analyzing func work", 0);
        return -1;
    }

    Node_Child parent_node = added_nodes->back();
    parent_node.child_amount++;

    if      (parent_node.child_amount == 1) ADD_CHILD((*tree), *parent_node.node, *ready_node.node, -1)
    else if (parent_node.child_amount == 2) ADD_CHILD((*tree), *parent_node.node, *ready_node.node,  1)
    else return i;                  // Incorrect parent child amount

    added_nodes->pop_back();
    added_nodes->push_back(parent_node);
    // ------------------------------------------------------------------------
    
    *shift = i - start_pos;
    LOG2(analyze_func_debug(data, i, added_nodes, "Check updating children"););
    return -1;
}

int analyze_func_debug(const char* data, int index, std::list<Node_Child>* added_nodes, const char* reason) {
    ASSERT_IF(VALID_PTR(data),        "Invalid data ptr",        0);
    ASSERT_IF(VALID_PTR(added_nodes), "Invalid added_nodes ptr", 0);
    ASSERT_IF(VALID_PTR(reason),      "Invalid reason ptr",      0);

    printf("Reason: %s\n", reason);

    int len = strlen(data);
    for (int i = 0; i < len; i++) {
        if (i != index) printf("%c", data[i]);
        else            printf(GREEN "%c" NATURAL, data[i]);
    }
    printf("\n\n");

    printf("Nodes: [ ");
    for (Node_Child cur_node : *added_nodes) {
        printf("'%5s' ", cur_node.node->data);
    }
    printf("]\n");

    printf("Child: [ ");
    for (Node_Child cur_node : *added_nodes) {
        printf(" %5d  ", cur_node.child_amount);
    }
    printf("]\n********************************************************************************\n\n");

    return 1;
}