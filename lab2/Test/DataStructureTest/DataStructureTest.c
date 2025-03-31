#include "symboltable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Helper function to print a type
void print_type(Type *type) {
    switch (type->kind) {
        case BASIC:
            printf("Basic Type: %d\n", type->u.basic);
            break;
        case ARRAY:
            printf("Array Type: Element Type=%d, Size=%d\n", type->u.array.elem->u.basic, type->u.array.size);
            break;
        case STRUCTURE:
            printf("Structure Type\n");
            FieldList *field = type->u.structure;
            while (field != NULL) {
                printf("  Field Name: %s, ", field->name);
                print_type(field->type);
                field = field->nextFieldList;
            }
            break;
        case FUNCTION:
            printf("Function Type: Return Type=");
            print_type(type->u.function.returntype);
            printf("Parameters:\n");
            FieldList *param = type->u.function.params;
            while (param != NULL) {
                printf("  Parameter Name: %s, ", param->name);
                print_type(param->type);
                param = param->nextFieldList;
            }
            break;
    }
}

int main() {
    // Create and test HashTable
    HashTable *hashTable = create_hashtable();
    ScopeHeadStack *scopeStack = create_scopeheadstack();

    // Create some types for testing
    Type intType = {.kind = BASIC, .u.basic = 0};
    Type floatArrayType = {.kind = ARRAY, .u.array = {.elem = &intType, .size = 10}};
    
    FieldList field1 = {.name = strdup("x"), .type = &intType, .nextFieldList = NULL};
    FieldList field2 = {.name = strdup("y"), .type = &floatArrayType, .nextFieldList = &field1};
    Type structureType = {.kind = STRUCTURE, .u.structure = &field2};

    // Create symbol table nodes
    SymbolTableNode node1 = {.type = &intType, .name = strdup("var1"), .kind = VAR, .isdef = true, .nextBucketNode = NULL, .nextScopeNode = NULL};
    SymbolTableNode node2 = {.type = &structureType, .name = strdup("struct1"), .kind = STRUCTURE, .isdef = true, .nextBucketNode = NULL, .nextScopeNode = NULL};

    // Push scope and insert nodes
    push_scope(hashTable, scopeStack, &node1);
    push_scope(hashTable, scopeStack, &node2);

    // Get and print nodes
    SymbolTableNode *retrievedNode1 = get_symboltable_node(hashTable, scopeStack, "var1");
    if (retrievedNode1 != NULL) {
        printf("Retrieved Node: %s, Kind: %d, Is Defined: %d\n", retrievedNode1->name, retrievedNode1->kind, retrievedNode1->isdef);
        print_type(retrievedNode1->type);
    }

    SymbolTableNode *retrievedNode2 = get_symboltable_node(hashTable, scopeStack, "struct1");
    if (retrievedNode2 != NULL) {
        printf("Retrieved Node: %s, Kind: %d, Is Defined: %d\n", retrievedNode2->name, retrievedNode2->kind, retrievedNode2->isdef);
        print_type(retrievedNode2->type);
    }

    // Pop scope
    pop_scope(hashTable, scopeStack);

    // Test Function Table
    FunctionTable *functionTable = create_functiontable();
    insert_functiontable_node(functionTable, strdup("func1"), 5);
    insert_functiontable_node(functionTable, strdup("func2"), 10);

    FunctionTable *retrievedFunc1 = get_functiontable_node(functionTable, "func1");
    if (retrievedFunc1 != NULL) {
        printf("Retrieved Function: %s, Line Number: %d\n", retrievedFunc1->name, retrievedFunc1->lineNo);
    }

    // Test Structure Table
    StructureTable *structureTable = create_structuretable_head();
    insert_structuretable_node(structureTable, &node2);

    StructureTable *retrievedStruct1 = get_structuretable_node(structureTable, "struct1");
    if (retrievedStruct1 != NULL) {
        printf("Retrieved Structure: %s\n", retrievedStruct1->structNode->name);
        print_type(retrievedStruct1->structNode->type);
    }

    // Free allocated memory
    free(node1.name);
    free(node2.name);
    free(field1.name);
    free(field2.name);

    return 0;
}



