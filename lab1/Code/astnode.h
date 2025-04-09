#ifndef __ASTNODE_H__
#define __ASTNODE_H__

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#define MAX_CHILDREN_NUM 16
/* according to the guidance: less than 32btis number */
#define MAX_OCT_BITWIDTH 12 
#define MAX_HEX_BITWIDTH 10

enum nodetype
{
    LEX_NODE,
    SYN_NODE
};

typedef struct node {
    int lineno;                             /* the line num of this node*/
    char name[32];                          /* e.g. TYPE , make sure that name is less than 32*/
    /* Attention: the INT and FLOAT is stored as a char array (string) */
    char value[32];                         /* e.g. int , make sure that value is less than 32*/
    //struct node *parent;                    /* only one parent */ don't need parent
    struct node *childs[MAX_CHILDREN_NUM];  /* possibly many childs, but must be less than MAX_CHILDREN_NUM*/
    int childnum;                           /* the number of childs */
    int nodetype;
} Node;

Node *create_node(int nodetype, int lineno, char *name, char *value, int childnum, ...);
void print_tree(Node *root, int depth);
int str2int(char *value);
float str2flt(char *value);
#endif