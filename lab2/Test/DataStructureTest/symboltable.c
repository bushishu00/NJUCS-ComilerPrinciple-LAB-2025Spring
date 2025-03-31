#include "symboltable.h"

/* Hash Function from guidance */
unsigned int hash_pjw(char *name){
    unsigned int val = 0, i;
    for (; *name; ++name)
    {
        val = (val << 2) + *name;
        if (i = val & ~TABLESIZE)
            val = (val ^ (i >> 12)) & TABLESIZE;
    }
    return val;
}

/**********************************************************************************
 * Scope Stack
 * 1. Create a new stack
 * 2. Push a new scope onto the stack and linked to the hash table
 * 3. Pop a scope from the stack and delete from the hash table
 * 
!!Attention!!: 
Push means we insert the head node of the scope into the hash table.
So the insert function below don't mention insert the head node into the hash table.
***********************************************************************************/
/* Create a Scope Stack */
ScopeHeadStack* create_scopeheadstack(){
    ScopeHeadStack* stack = (ScopeHeadStack*)malloc(sizeof(ScopeHeadStack));
    stack->top = NULL;
    return stack;
}

/* Push a new scope onto the stack and linked to the hash table */
void push_scope(HashTable *hashtable, ScopeHeadStack *stack, SymbolTableNode *headNode) {
    /* push scope head */
    ScopeHead *newScope = (ScopeHead*)malloc(sizeof(ScopeHead));
    newScope->ScopeHeadNode = headNode; // set the head of the new scope
    newScope->nextScope = stack->top; // link to the previous top scope
    stack->top = newScope; // update the top of the stack

    /* link scope head node to hash table */
    headNode->nextBucketNode = hashtable->buckets[hash_pjw(headNode->name)]; 
    hashtable->buckets[hash_pjw(headNode->name)] = headNode;
}

/* Pop a scope from the stack and delete from the hash table */
void pop_scope(HashTable *hashtable, ScopeHeadStack *stack) {
    if (stack->top == NULL) {
        printf("Error: No scope to pop\n");
        return;
    }
    // pop the top scope from the stack
    ScopeHead *topScope = stack->top; 
    stack->top = topScope->nextScope; 

    // delete the scope from the hash table
    SymbolTableNode *curNode = topScope->ScopeHeadNode;
    while (curNode != NULL) {
        hashtable->buckets[hash_pjw(curNode->name)] = curNode->nextBucketNode;
        SymbolTableNode *nextNode = curNode->nextScopeNode; 
        curNode = nextNode; 
    }
}

/**********************************************************************************
 * Hash Table
 * 1. Create a new hash table
 * 2. Create a new symbol table node
 * 3. Insert a symbol table node (non scope head node) into the hash table
 * 4. Look up a symbol table node in the hash table
 
 actually, we don't need to delete a node from the hash table, because we implement this by scope stack.
**********************************************************************************/

/* Create a Hash table */
HashTable* create_hashtable(){
    HashTable* table = (HashTable*)malloc(sizeof(HashTable));
    assert(table != NULL);/* table is very large, so we make sure we have enough space*/
    for(int i = 0; i < TABLESIZE; ++i) {
        table->buckets[i] = NULL;
    }
    return table;
}

/* Create a new Symbol table node */
SymbolTableNode* create_symboltable_node(Type *type, char *name, int kind, bool isdef){
    SymbolTableNode *ret = (SymbolTableNode*)malloc(sizeof(SymbolTableNode));
    ret->type = type;
    ret->name = name;
    ret->kind = kind;
    ret->isdef = isdef;
    ret->nextBucketNode = NULL;
    ret->nextScopeNode = NULL;
    return ret;
}

/* Insert a Symbol table node (non scope head node) into a Hash Table */
void insert_symboltable_node(HashTable *hashtable, ScopeHeadStack *stack, SymbolTableNode *insertNode) {
    /* if NULL, the node should be inserted by PUSH STACK */
    assert(stack->top != NULL);

    /* find the previous scope node */
    SymbolTableNode *curScopeNode = stack->top->ScopeHeadNode;
    while(curScopeNode->nextScopeNode != NULL) {
        curScopeNode = curScopeNode->nextScopeNode;
    }
    /* table/stack connect to node */
    hashtable->buckets[hash_pjw(insertNode->name)] = insertNode;   
    curScopeNode->nextScopeNode = insertNode;
    /* node connect to stack/table*/
    insertNode->nextBucketNode = hashtable->buckets[hash_pjw(insertNode->name)];
    insertNode->nextScopeNode = NULL;
}

/* look up a hash table node */
SymbolTableNode* get_symboltable_node(HashTable *hashtable, ScopeHeadStack *stack, char *name){
    /* search in the bucket */
    SymbolTableNode *curBucketNode = hashtable->buckets[hash_pjw(name)];
    SymbolTableNode *ret_bucket = NULL;
    while(curBucketNode != NULL) {
        /* the first is the correct, because we insert at head */
        if (strcmp(curBucketNode->name, name) == 0) {
            ret_bucket = curBucketNode;
            break;
        }
        curBucketNode = curBucketNode->nextBucketNode;
    }

    if (ret_bucket == NULL) {
        printf("Error: %s not found in the symbol table\n", name);
        return NULL; 
    }

    /* search in the scope */
    /* TODO: delete the code after test */
    SymbolTableNode *curScopeNode = stack->top->ScopeHeadNode;
    SymbolTableNode *ret_scope = NULL;
    while(curScopeNode != NULL) {
        if (strcmp(curScopeNode->name, name) == 0) {
            ret_scope = curScopeNode;
            break;
        }
        curScopeNode = curScopeNode->nextScopeNode;
    }

    /* these tow rets should be the same */
    if (ret_scope != NULL && ret_bucket != NULL) {
        assert(ret_bucket == ret_scope);
    }

    return ret_bucket;
}

/**********************************************************************************
 * Function Table
 * 1. Create and initialize a function table
 * 2. Insert a function table node into the function table
 * 3. Look up a function table node in the function table
 
struct FunctionTable {
    char *name;
    int lineNo;
    FunctionTable *nextFun;
};
**********************************************************************************/
/* Create a function table , return a head ptr*/
FunctionTable* create_functiontable(){
    FunctionTable* head = (FunctionTable*)malloc(sizeof(FunctionTable));
    head->name = NULL;
    head->lineNo = 0;
    head->nextFun = NULL;
    return head;
}

/* Insert(and create) a function table node into the function table (head insert)*/
void insert_functiontable_node(FunctionTable *head, char *name, int lineNo) {
    FunctionTable *newNode = (FunctionTable*)malloc(sizeof(FunctionTable));
    newNode->name = name;
    newNode->lineNo = lineNo;
    newNode->nextFun = head->nextFun;
    head->nextFun = newNode;
}

/* Look up a function table node in the function table */
FunctionTable* get_functiontable_node(FunctionTable *head, char *name) {
    FunctionTable *curNode = head->nextFun;
    while(curNode != NULL) {
        if (strcmp(curNode->name, name) == 0) {
            return curNode;
        }
        curNode = curNode->nextFun;
    }
    return NULL;
}

/**********************************************************************************
 * Struture Table
 * 1. Create a new Struture table
 * 2. Insert a Struture table node into the Struture table
 * 3. Look up a Struture table node in the Struture table
 
struct StructureTable {
    SymbolTableNode *structNode;
    StructureTable *nextStruct;
};
**********************************************************************************/
/* Create a structure table */
StructureTable* create_structuretable_head(){
    StructureTable* head = (StructureTable*)malloc(sizeof(StructureTable));
    head->structNode = NULL;
    head->nextStruct = NULL;
    return head;
}

/* Insert a Struture table node into the Struture table */
void insert_structuretable_node(StructureTable *head, SymbolTableNode *structNode) {
    StructureTable *newNode = (StructureTable*)malloc(sizeof(StructureTable));
    newNode->structNode = structNode;
    newNode->nextStruct = head->nextStruct;
    head->nextStruct = newNode;
}
/* Look up a Struture table node in the Struture table */
StructureTable* get_structuretable_node(StructureTable *head, char *name) {
    StructureTable *curNode = head->nextStruct;
    while(curNode != NULL) {
        if (strcmp(curNode->structNode->name, name) == 0) {
            return curNode;
        }
        curNode = curNode->nextStruct;
    }
    return NULL;
}