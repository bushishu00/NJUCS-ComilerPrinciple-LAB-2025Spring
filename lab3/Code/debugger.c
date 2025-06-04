#include "debugger.h"

void print_procedure(astNode *root, int DEBUGMODE){
    if (DEBUGMODE == 1) {
        if (root == NULL) {
            printf("Error: root is NULL! \n");
            return;
        }
        printf("Analyzing: %s ->", root->value);
        for(int i = 0; i < 6; i++){
            if (get_child(root, i) != NULL) {
                printf(" %s(node%d)", get_child(root, i)->value, i);
            }
        }
        printf(" at line %d\n", root->lineno);
    }
    
}

void print_scope_stack(ScopeStack *stack, int DEBUGMODE){
    if (DEBUGMODE == 1) {
        if (stack == NULL) {
            printf("Scope stack is empty.\n");
            return;
        }
    
        ScopeHead *currentScope = stack->top;
        int level = 0;
        while (currentScope != NULL) {
            printf("Scope Level: %d\n", level++);
            SymbolTableNode *currentNode = currentScope->ScopeHeadNode;
            while (currentNode != NULL) {
                printf("Symbol: %s, Kind: %d\n", currentNode->name, currentNode->kind);
                currentNode = currentNode->nextScopeNode;
            }
            currentScope = currentScope->nextScope;
        }
    }
    
}

void print_symbol_table(HashTable *table, int DEBUGMODE){
    if (DEBUGMODE == 1) {
        if (table == NULL) {
            printf("Symbol table is empty.\n");
            return;
        }
    
        for (int i = 0; i < TABLESIZE; i++) {
            SymbolTableNode *currentNode = table->buckets[i];
            while (currentNode != NULL) {
                printf("Symbol: %s, Kind: %d\n", currentNode->name, currentNode->kind);
                currentNode = currentNode->nextBucketNode;
            }
        }   
    }
    
}

void print_struct_table(StructureTable *table, int DEBUGMODE) {
    if (DEBUGMODE == 1) {
        if (table == NULL) {
            printf("Structure table is empty.\n");
            return;
        }

        printf("Structure Table:\n");
        StructureTable *currentStruct = table;
        while (currentStruct != NULL) {
            SymbolTableNode *structNode = currentStruct->structNode;
            if (structNode != NULL) {
                printf("Struct Name: %s\n", structNode->name);
                printf("Fields:\n");
                FieldList *fields = structNode->type->u.structure.structures;
                while (fields != NULL) {
                    printf("  Field Name: %s, Field Type: ", fields->name);
                    type_print(fields->type, DEBUGMODE); // 打印字段类型
                    fields = fields->nextFieldList;
                }
            }
            currentStruct = currentStruct->nextStruct;
        }
    }
}

void print_function_table(FunctionTable *table, int DEBUGMODE) {
    if (DEBUGMODE == 1) {
        if (table == NULL) {
            printf("Function table is empty.\n");
            return;
        }

        printf("Function Table:\n");
        FunctionTable *currentFunction = table;
        while (currentFunction != NULL) {
            printf("Function Name: %s\n", currentFunction->name);
            printf("Defined: %s\n", currentFunction->isdef ? "Yes" : "No");
            printf("Line Number: %d\n", currentFunction->lineNo);
            printf("Return Type:\n");
            type_print(currentFunction->type->u.function.returntype, DEBUGMODE); // 打印返回值类型
            printf("Parameters (%d):\n", currentFunction->type->u.function.paraNum);
            FieldList *params = currentFunction->type->u.function.params;
            while (params != NULL) {
                printf("  Parameter Name: %s, Parameter Type: ", params->name);
                type_print(params->type, DEBUGMODE); // 打印参数类型
                params = params->nextFieldList;
            }
            currentFunction = currentFunction->nextFun;
        }
    }
}

void field_print(FieldList *field, int DEBUGMODE){
    if (DEBUGMODE == 1) {
        if (field == NULL)
        return;
        FieldList *curField = field;
        while (curField != NULL){
            printf("Field name: %s, Field kind: %d\n", curField->name, curField->type->kind);
            curField = curField->nextFieldList;
        } 
    }
}

void type_print(Type *type, int DEBUGMODE){
    if (DEBUGMODE == 1) {
        if (type == NULL) {
            printf("Type: NULL\n");
            return;
        }
    
        switch (type->kind) {
            case BASIC:
                if (type->u.basic == 0) {
                    printf("Type: BASIC (int)\n");
                } else if (type->u.basic == 1) {
                    printf("Type: BASIC (float)\n");
                } else {
                    printf("Type: BASIC (unknown)\n");
                }
                break;
    
            case ARRAY: {
                printf("Type: ARRAY\n");
                int dimension = 0;
                Type *curType = type;
                while (curType != NULL && curType->kind == ARRAY) {
                    printf("  Dimension %d: size = %d\n", ++dimension, curType->u.array.size);
                    curType = curType->u.array.elem;
                }
                printf("  Element Type:\n");
                type_print(curType, DEBUGMODE); // 打印数组的元素类型
                break;
            }
    
            case STRUCTURE: {
                printf("Type: STRUCTURE\n");
                if (type->u.structure.name != NULL) {
                    printf("  Struct Name: %s\n", type->u.structure.name);
                } else {
                    printf("  Anonymous Struct\n");
                }
                printf("  Fields:\n");
                FieldList *field = type->u.structure.structures;
                while (field != NULL) {
                    printf("    Field Name: %s\n", field->name);
                    printf("    Field Type:\n");
                    type_print(field->type, DEBUGMODE); // 递归打印字段类型
                    field = field->nextFieldList;
                }
                break;
            }
    
            case FUNCTION: {
                printf("Type: FUNCTION\n");
                printf("  Return Type:\n");
                type_print(type->u.function.returntype, DEBUGMODE); // 打印返回值类型
                printf("  Parameters (%d):\n", type->u.function.paraNum);
                FieldList *param = type->u.function.params;
                while (param != NULL) {
                    printf("    Parameter Name: %s\n", param->name);
                    printf("    Parameter Type:\n");
                    type_print(param->type, DEBUGMODE); // 递归打印参数类型
                    param = param->nextFieldList;
                }
                break;
            }
    
            default:
                printf("Type: UNKNOWN\n");
                break;
        }
    }
}
