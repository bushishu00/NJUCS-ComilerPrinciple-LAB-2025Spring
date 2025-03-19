#include "mynode.h"

Node* create_node(int lineno, char* name, char* value, int nodetype){
    Node* node = (Node*)malloc(sizeof(Node));/* create a node */
    assert(node != NULL);
    node->lineno = lineno;
    strcpy(node->name, name);
    strcpy(node->value, value);
    node->parent = NULL;
    for(int i; i < MAX_CHILDREN_NUM; i++){
        node->childs[i] = NULL;
    }
    node->childnum = 0;
    node->nodetype = nodetype;
    return node;
}

/* update: syntax need to add multi childs, we use <stdarg.h> to utilize the function*/
void add_child(Node* parent, Node* child){
    assert(parent->childnum < MAX_CHILDREN_NUM);
    parent->childs[parent->childnum] = child;
    parent->childnum++;
    child->parent = parent;
}

void add_many_childs(Node* parent, int childnum, ...){
    va_list children;
    va_start(children, childnum);
    for(int i = 0; i < childnum; i++){
        Node* child = va_arg(children, Node*);
        add_child(parent, child);
    }
    va_end(children);
}

/* print the whole Abstract Tree **PreOrderly** */
void print_tree(Node* root, int level){/* root is the parent node, level is the current level of the tree, e.g. root's level is 0*/
    if (root->nodetype == SYN_NODE && root->childnum == 0){/* product empty */
        return;
    }
    for (int i=0; i<level; i++){/* use space to seperate each level */
        printf("  ");/* 2 space */
    }
    if (root->childnum != 0){/* not the leaf node */
        printf("%s (%d)\n", root->name, root->lineno);
        for(int i=0; i<root->childnum; i++){
            print_tree(root->childs[i], level+1);
        }
    }else{/* leaf node, print depends on the node type */
        /* only TYPE, INT, FLOAT and ID need to be print specially */
        if (strcmp(root->name, "TYPE") == 0){
            printf("%s: %s\n", root->name, root->value);
        }
        else if (strcmp(root->name, "INT") == 0){
            /* create another 2 patterns to recognize the oct and hex*/
            printf("%s: %d\n", root->name, atoi(root->value));
        }
        else if (strcmp(root->name, "INT_oct") == 0){
            int val = 0;
            sscanf(root->value, "%o", &val);
            printf("%s: %d\n", "INT", val);
        }
        else if (strcmp(root->name, "INT_hex") == 0){
            int val = 0;
            sscanf(root->value, "%x", &val);
            printf("%s: %d\n", "INT", val);
        }
        else if (strcmp(root->name, "FLOAT") == 0){
            printf("%s: %f\n", root->name, atof(root->value));
        }
        else if (strcmp(root->name, "FLOAT_exp") == 0){
            float val = 0;
            sscanf(root->value, "%f", &val);
            printf("%s: %f\n", "FLOAT", val);
        }
        else if (strcmp(root->name, "ID") == 0){
            printf("%s: %s\n", root->name, root->value);
        }
        else{
            printf("%s\n", root->name);
        }
    }
}
