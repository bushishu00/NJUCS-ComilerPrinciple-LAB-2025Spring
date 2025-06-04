#ifndef __DEBUGGER_H__
#define __DEBUGGER_H__

#include "astnode.h"
#include "intermediate.h"
#include "symboltable.h"
#include <stdio.h>
void print_scope_stack(ScopeStack *stack, int DEBUGMODE);
void print_symbol_table(HashTable *table, int DEBUGMODE);
void print_procedure(astNode *root, int DEBUGMODE);
void print_struct_table(StructureTable *table, int DEBUGMODE);
void print_function_table(FunctionTable *table, int DEBUGMODE);
void field_print(FieldList *field, int DEBUGMODE);
void type_print(Type *type, int DEBUGMODE);

#endif