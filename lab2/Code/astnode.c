#include "astnode.h"

astNode *create_node(int nodetype, int lineno, char *name, char *value, int childnum, ...){
    astNode *ret = (astNode*)malloc(sizeof(astNode));
    assert(ret != NULL);
    ret->lineno = lineno;
    ret->childnum = childnum;
    ret->nodetype = nodetype;
    strcpy(ret->name, name);
    strcpy(ret->value, value);

    for(int i = 0; i < MAX_CHILDREN_NUM; i++){
        ret->childs[i] = NULL;
    }

    if (childnum >= 1){/* nonterminal */
        va_list children;
    va_start(children, childnum);
    for(int i = 0; i < childnum; i++){
        astNode *child = va_arg(children, astNode*);
        ret->childs[i] = child;
    }
    va_end(children);
    }
    
    return ret;
}

astNode *get_child(astNode *parent, int index){
    if (parent == NULL || index < 0 || index >= parent->childnum){
        return NULL;
    }
    return parent->childs[index];
}

/* print the whole Abstract Tree **PreOrderly** */
void print_tree(astNode *root, int depth){/* root is the parent node, depth is the current depth of the tree, e.g. root's depth is 0*/
    if (root == NULL) {
        return;
    }
    
    for (int i=0; i<depth; i++){/* use space to seperate each depth */
        printf("  ");/* 2 spaces */
    }
    if (root->childnum != 0){/* not the leaf node */
        printf("%s (%d)\n", root->name, root->lineno);
        for(int i=0; i<root->childnum; i++){
            print_tree(root->childs[i], depth+1);
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