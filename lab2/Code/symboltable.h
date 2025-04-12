#ifndef __SYMBOLTABLE_H__
#define __SYMBOLTABLE_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define bool int
#define true 1
#define TRUE 1
#define false 0
#define FALSE 0
#define TABLESIZE 0x3fff//to be modified

typedef struct Type_ Type;
typedef struct FieldList_ FieldList;
typedef struct SymbolTableNode SymbolTableNode;
typedef struct HashTable HashTable;
typedef struct ScopeHead ScopeHead;
typedef struct ScopeStack ScopeStack;
typedef struct FunctionTable FunctionTable;
typedef struct StructureTable StructureTable;


struct Type_ {
    enum { 
        BASIC, 
        ARRAY,
        STRUCTURE, 
        FUNCTION 
    } kind;
    union {
        int basic; //basic type: int, float
        //array type: int[10], float[20]
        struct { 
            Type *elem; 
            int size; 
        } array;
        // struct type is a linked list of fields
        struct
        { 
            char *name;
            FieldList *structures;
        } structure;
        struct { 
            int paraNum;
            FieldList *params;
            Type *returntype;
        } function;// function parameter list}
    } u;
};

/* Use Linked List to implement a Fieldlist */
struct FieldList_ {
    char *name; // field name
    Type *type; // field type
    FieldList *nextFieldList; // next field in the list
};

/* a cross linked-list */
/* the Hash table only store the symbol itself.
   but the scope is maintained by the 'nextScopeNode' list*/
struct SymbolTableNode {
    Type *type;                      
    char *name;                     
    int kind;                       // 0 for variable, 1 for struct, 2 for function
    SymbolTableNode *nextBucketNode;  // next node in the symbol table, linked list insered at the head
    SymbolTableNode *nextScopeNode;  // next node in the same scope
};

struct ScopeHead {
    SymbolTableNode *ScopeHeadNode;           // hash table for this scope
    ScopeHead *nextScope;                      // next scope in the stack
};

struct ScopeStack {
    ScopeHead *top;                           // top of the stack
};

/* Hash Table is an array of bucket(pointer) */
struct HashTable {
    SymbolTableNode *buckets[TABLESIZE];
};

/* Function Table used to record functions */
struct FunctionTable {
    char *name;
    int lineNo;
    bool isdef;
    Type *type;
    FunctionTable *nextFun;
};

/* Structure Table used to record Structures */ 
struct StructureTable {
    SymbolTableNode *structNode;
    StructureTable *nextStruct;
};



unsigned int hash_pjw(char *name);

ScopeStack* init_scopestack(void);
void push_scope(ScopeStack *stack);
void pop_scope(HashTable *hashtable, ScopeStack *stack);
SymbolTableNode* get_scope_node(ScopeStack *stack, char *name);

HashTable* init_hashtable(void);
SymbolTableNode* create_symboltable_node(Type *type, char *name, int kind);
void insert_symboltable_node(HashTable *hashtable, ScopeStack *stack, SymbolTableNode *insertNode);
SymbolTableNode* get_symboltable_node(HashTable *hashtable, char *name);

FunctionTable* init_functiontable_head(void);
void insert_functiontable_node(FunctionTable *head, char *name, int lineNo, bool isdef, Type *type);
FunctionTable* get_functiontable_node(FunctionTable *head, char *name);

StructureTable* init_structuretable_head(void);
void insert_structuretable_node(StructureTable *head, SymbolTableNode *structNode);
StructureTable* get_structuretable_node(StructureTable *head, char *name);


#endif