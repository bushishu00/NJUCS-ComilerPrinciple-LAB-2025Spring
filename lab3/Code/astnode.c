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
    } else{/* leaf node, print depends on the node type */
        /* only TYPE, INT, FLOAT and ID need to be print specially */
        if (strcmp(root->name, "TYPE") == 0){
            printf("%s: %s\n", root->name, root->value);
        }
        else if (strcmp(root->name, "INT") == 0){
            /* create another 2 patterns to recognize the oct and hex*/
            printf("%s: %d\n", root->name, str2int(root->value));
        }
        else if (strcmp(root->name, "FLOAT") == 0){
            printf("%s: %f\n", root->name, str2flt(root->value));
        }
        else if (strcmp(root->name, "ID") == 0){
            printf("%s: %s\n", root->name, root->value);
        }
        else{
            printf("%s\n", root->name);
        }
    }
}

int str2int(char *value) {
    if (value == NULL) {
        return 0;
    }
    if (value[0] == '0') {
        if (value[1] == 'x' || value[1] == 'X') {
            // hex
            int val = 0;
            sscanf(value, "%x", &val);
            return val;
        } else if (isdigit(value[1])) {
            // oct
            int val = 0;
            sscanf(value, "%o", &val);
            return val;
        }
    }
    // dec
    return atoi(value);
}

float str2flt(char *value) {
    if (value == NULL) {
        return 0.0f;
    }
    float val = 0.0f;
    sscanf(value, "%f", &val); 
    return val;
}