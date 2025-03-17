#include "mynode.h"

Node* create_node(int lineno, char* name, char* value){
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
    return node;
}

void add_child(Node* parent, Node* child){
    assert(parent->childnum < MAX_CHILDREN_NUM);/* child array is not overflow */
    /* connect the parent node and the child node */
    parent->childs[parent->childnum] = child;
    parent->childnum++;
    child->parent = parent;
    /* TODO: How lineno change? */
}

/* print the whole Abstract Tree **PreOrderly** */
void print_tree(Node* root, int level){/* root is the parent node, level is the current level of the tree, e.g. root's level is 0*/
    if (root == NULL){
        return;
    }
    for (int i=0; i<level; i++){/* use space to seperate each level */
        printf(" ");
    }
    if (root->childnum != 0){/* not the leaf node */
        printf("%s (%d)\n", root->name, root->lineno);
        for(int i=0; i<root->childnum; i++){
            print_tree(root->childnum[i], level+1);
        }
    }else{/* leaf node, print depends on the node type */
        /* p.s. the return value of function strcmp: 
            s1 < s2 neg
            s1 = s2 0
            s1 > s2 pos
        */
        /* Attention: only INT, FLOAT and ID need to be print specially */
        if (strcmp(root->name, "INT") == 0){
            /* TODO: how to print oct and hex integer */
            printf("%s: %d\n", root->name, atoi(root->value));
        }
        else if (strcmp(root->name, "FLOAT") == 0){
            /* TODO: how to print expfloat */
            printf("%s: %d\n", root->name, atof(root->value));
        }
        else if (strcmp(root->name, "ID") == 0){
            printf("%s: %s\n", root->name, root->value);
        }
        else{
            printf("%s\n", root->name);
        }
    }
}

