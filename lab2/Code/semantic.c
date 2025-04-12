#include "semantic.h"

/* Global Var */
HashTable *symbolTable = NULL;
ScopeStack *scopeStack = NULL;
FunctionTable *functionTable = NULL;
StructureTable *structTable = NULL;
#define DEBUGMODE 0
#define SHOW_PROCESS 0
#define breakpoint printf("----------------------------\n");

void debug_print(astNode* root, int debug){
    if (debug == 1){
        printf("Parsing %s: %s at line: %d \n", root->name, root->value, root->lineno);
    }
    return ;
}

void child_print(astNode* root, int debug){
    if (debug == 1){
        for (int i=0; i<7; i++){
            astNode *nodei = get_child(root, i);
            if (nodei != NULL)
                printf("%s\n", nodei->name);
            else
                printf("the node%d is NULL\n", i);
        }
    }
}

void field_print(FieldList *field){
    if (field == NULL)
        return;
    FieldList *curField = field;
    while (curField != NULL){
        printf("Field name: %s, Field kind: %d\n", curField->name, curField->type->kind);
        curField = curField->nextFieldList;
    }
}

void type_print(Type *type) {
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
            type_print(curType); // 打印数组的元素类型
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
                type_print(field->type); // 递归打印字段类型
                field = field->nextFieldList;
            }
            break;
        }

        case FUNCTION: {
            printf("Type: FUNCTION\n");
            printf("  Return Type:\n");
            type_print(type->u.function.returntype); // 打印返回值类型
            printf("  Parameters (%d):\n", type->u.function.paraNum);
            FieldList *param = type->u.function.params;
            while (param != NULL) {
                printf("    Parameter Name: %s\n", param->name);
                printf("    Parameter Type:\n");
                type_print(param->type); // 递归打印参数类型
                param = param->nextFieldList;
            }
            break;
        }

        default:
            printf("Type: UNKNOWN\n");
            break;
    }
}

void scope_print() {
    printf("\nThe scope has symbols below:\n");
    if (scopeStack->top == NULL) {
        printf("Scope stack is empty.\n");
        return;
    }

    if (scopeStack->top->ScopeHeadNode!=NULL){
        SymbolTableNode *currentNode = scopeStack->top->ScopeHeadNode;
        int i = 1;
        while (currentNode != NULL) {
            printf("Symbol: %s, Kind: %d, ", currentNode->name, currentNode->kind);
            type_print(currentNode->type);
            currentNode = currentNode->nextScopeNode;
        }
    } else {
        printf("Nothing in current Scope\n");
    }
    printf("\n");
}

/* Check By myself */
void Program(astNode *root){
    /*
    Program -> ExfDefList
    */
    debug_print(root, DEBUGMODE);

    symbolTable     = init_hashtable();
    scopeStack      = init_scopestack();
    functionTable   = init_functiontable_head();
    structTable     = init_structuretable_head();
    /* Scope: Global */
    scope_begin();
    ExtDefList(get_child(root, 0));
    check_undef_func();
    if (SHOW_PROCESS == 1)
        scope_print();
    scope_end();
}

/* Check By myself */
void ExtDefList(astNode *root){
    /*
    ExfDefList -> ExfDef ExfDefList
    | (empty)
    */
    debug_print(root, DEBUGMODE);

    astNode *node0 = get_child(root, 0);
    astNode *node1 = get_child(root, 1);
    if (node0 != NULL && node1 != NULL)
    {
        ExtDef(node0);
        ExtDefList(node1);
    }
    if (node0 != NULL && node1 == NULL)
    {
        ExtDef(node0);
    }
}

/* Check By myself */
void ExtDef(astNode *root){
    /*
    ExtDef -> Specifier ExtDecList SEMI
    | Specifier SEMI
    | Specifier FunDec CompSt
    | Specifier FunDec SEMI
    */
    debug_print(root, DEBUGMODE);

    astNode *node0 = get_child(root, 0);
    astNode *node1 = get_child(root, 1);
    astNode *node2 = get_child(root, 2);
    Type *type = Specifier(node0);// node0 mustn't be NULL, which is make sure by syntax

    if (strcmp(node1->name, "ExtDecList")==0){
        /* Specifier ExtDecList SEMI */
        ExtDecList(node1, type);
    } 
    else if (strcmp(node1->name, "FunDec")==0) {
        if (strcmp(node2->name, "SEMI")==0)
        {
            /* Specifier FunDec SEMI */
            /* THIS IS REAL Declaration */
            scope_begin();// a new scope of FUNCTION begin at Formal Param
            FunDec(node1, FALSE, type);
            scope_end();
        }
        else
        {
            /* Specifier FunDec CompSt */
            /* THIS IS REAL Definition */
            scope_begin();// a new scope of FUNCTION begin at Formal Param
            FunDec(node1, TRUE, type);
            CompSt(node2, type);
            scope_end();
        }
    }
    else {
        /* Specifier SEMI */
        return;// meaningless behavior
    }
}

/* Check By myself */
void ExtDecList(astNode *root, Type *type){
    /*
    ExtDecList -> VarDec
    | VarDec COMMA ExtDecList
    */
    debug_print(root, DEBUGMODE);

    /* VarDec */
    FieldList *field = VarDec(get_child(root, 0), type);
    /* check redefine */
    if (check_var_redefined(field->name)){
        semantic_error(Redefined_Variable_Name, root->lineno, root->value);
    }
    SymbolTableNode *symbol = create_symboltable_node(field->type, field->name, 0);// kind = 0, var
    insert_symboltable_node(symbolTable, scopeStack, symbol);

    /* VarDec COMMA ExtDecList */
    astNode *node2 = get_child(root, 2);
    if (node2 != NULL){
        ExtDecList(node2, type);
    }
    return;
}

/* Check By myself */
Type *Specifier(astNode *root){
    /*
    Specifier -> TYPE
    | StructSpecifier
    */
    debug_print(root, DEBUGMODE);

    astNode *node0 = get_child(root, 0);
    Type *type = (Type*)malloc(sizeof(Type));
    if (strcmp(node0->name, "TYPE")==0){
        type->kind = BASIC;
        if (strcmp(node0->value, "int")==0)
            type->u.basic = 0;
        else if (strcmp(node0->value, "float")==0)
            type->u.basic = 1;
    } else {
        type = StructSpecifier(node0);
    }
    return type;
}


/* Check By myself */
Type *StructSpecifier(astNode *root){
    /*
    StructSpecifier -> STRUCT OptTag LC DefList RC
    | STRUCT Tag

    OptTag -> ID
    | (empty)

    Tag -> ID
    */
    debug_print(root, DEBUGMODE);

    Type *type = (Type*)malloc(sizeof(Type));
    type->kind = STRUCTURE;

    astNode *node1 = get_child(root, 1);// OptTag / Tag
    if (node1 == NULL){  // OptTag -> empty
        /* 没有ID的结构体不用插入结构表，也不用检查 */
        char *structName = NULL;
        type->u.structure.name = structName;
        astNode *node3 = get_child(root, 3);
        type->u.structure.structures = DefList_in_struct(node3);
    }
    else if (strcmp(node1->name, "OptTag")==0) { // STRUCT OptTag
        astNode *node10 = get_child(node1, 0);
        if (strcmp(node10->name, "ID")==0) { // OptTag -> ID 
            char *structName = node10->value;
            /* check structure name */
            int check = check_var_struct_redefined(structName, 1);
            if (check==4) {//4:结构体与结构体重名 仍然可以正常返回
                semantic_error(Redefined_Field_Name, node10->lineno, structName);
                type = get_structuretable_node(structTable, structName)->structNode->type; /* return the defined struct */
            } 
            else if (check==2) { // 2:结构体与变量重名
                semantic_error(Redefined_Field_Name, node10->lineno, structName);
                type = NULL;
            }
            else { //正在声明一个正确的结构体
                type->u.structure.name = structName;
                astNode *node03 = get_child(root, 3);
                type->u.structure.structures = DefList_in_struct(node03);
                insert_structuretable_node(structTable, create_symboltable_node(type, structName, 1));// insert a struct
            }
        }
    }
    
    else if (strcmp(node1->name, "Tag")==0)
    {
        astNode *IDnode = get_child(node1, 0);
        char *structName = IDnode->value;
        StructureTable *existStruct = get_structuretable_node(structTable, structName);
        /* Tag的使用是声明，没有定义，因此只检查未定义 */
        if (existStruct == NULL)
        {
            semantic_error(Undefined_Struct, IDnode->lineno, structName);
            // type = NULL;
        } else {
            type = existStruct->structNode->type;
        }
    }
    return type;
}
/***********************
 * 用于解析struct成员
 *******************/
FieldList *DefList_in_struct(astNode *root){
    /*
    DefList -> Def DefList
    | (empty)
    */
    debug_print(root, DEBUGMODE);
    if (root == NULL) {/* DefList -> NULL */
        return NULL;
    }
    astNode *node0 = get_child(root, 0);
    astNode *node1 = get_child(root, 1);
    if (node0 == NULL) {
        return NULL;
    }
    FieldList *field = Def_in_struct(node0);
    if (node1 != NULL) {
        FieldList *nxtField = DefList_in_struct(node1);
        /* 检查重名 */
        if (check_fieldlist_redefined(field, nxtField->name)){
            semantic_error(Redefined_Field, node1->lineno, nxtField->name);
        }
        /* TODO重名也连接，后续考虑删除重复声明 */
        /* 注意要先遍历到当前Def的最后一个，再连接 */
        FieldList *curField = field;
        while (curField->nextFieldList != NULL){
            curField = curField->nextFieldList;
        }
        curField->nextFieldList = nxtField;
    }
    return field;
}
FieldList *Def_in_struct(astNode *root){
    /*
    Def -> Specifier DecList SEMI
    */
    debug_print(root, DEBUGMODE);

    if (root == NULL) {
        return NULL;
    }
    astNode *spNode = get_child(root, 0);
    astNode *declistNode = get_child(root, 1);
    
    if (spNode != NULL && declistNode != NULL) {
        Type *tmptype = Specifier(spNode);
        return DecList_in_struct(declistNode, tmptype);
    }
    return NULL;
}
FieldList *DecList_in_struct(astNode *root, Type *type){
    /*
    DecList -> Dec
    | Dec COMMA DecList
    */
    debug_print(root, DEBUGMODE);
    astNode *node0 = get_child(root, 0);
    astNode *node2 = get_child(root, 2);

    FieldList *field = Dec_in_struct(node0, type);

    if (node2 != NULL) {
        /* 注意不要写成DefList_in_struct */
        FieldList *nxtField = DecList_in_struct(node2, type);
        /* 检查重名 */
        if (check_fieldlist_redefined(field, nxtField->name)){
            semantic_error(Redefined_Field, node2->lineno, nxtField->name);
        }
        field->nextFieldList = nxtField;
    }
    return field;
}
/* 和Dec的差别是，Dec可以直接查表检查重定义，但是域看不到前面的域名 */
FieldList *Dec_in_struct(astNode *root, Type *type){
    /*
    Dec -> VarDec
    Dec -> VarDec ASSIGNOP Exp是错误的初始化
    */
    debug_print(root, DEBUGMODE);
    astNode *node0 = get_child(root, 0);
    astNode *node1 = get_child(root, 1);
    astNode *node2 = get_child(root, 2);
    FieldList *varField = VarDec(node0, type);
    /* 即使赋值了，也继续认为Var是成员，不影响分析 */
    if (node1 != NULL) { // VarDec ASSIGNOP Exp
        /* 结构体内不允许初始化 */
        semantic_error(Redefined_Field, root->lineno, root->value);
    } 
    
    return varField;
}

/* Check By myself */   
FieldList *VarDec(astNode *root, Type *type){
    /*
    VarDec -> ID
    | VarDec LB INT RB
    */

    // 只负责识别变量，不负责识别重定义
    debug_print(root, DEBUGMODE);

    FieldList *field = (FieldList*)(malloc(sizeof(FieldList)));
    field->nextFieldList = NULL;/* VarDec only process one fieldlist */
    astNode *curNode = get_child(root, 0);
    if (curNode != NULL){
        if (strcmp(curNode->name, "ID")==0){/* Var */
            /* finish recursion */    
            field->name = curNode->value;
            field->type = type;
        }
        else if (strcmp(curNode->name, "VarDec")==0){/* Array */
            FieldList *nextField = VarDec(curNode, type);// return next level fieldlist
            Type *tmpType = (Type*)malloc(sizeof(Type));
            tmpType->kind = ARRAY;
            tmpType->u.array.elem = nextField->type;
            tmpType->u.array.size = str2int(get_child(root, 2)->value);
            field->name = nextField->name;/* name is all the same through each recursion*/
            field->type = tmpType;
        }
        return field;
    }
    assert(0);
}

/* Check By myself */   
int FunDec(astNode *root, bool isDefined, Type *type){
    /*
    FunDec -> ID LP VarList RP
    | ID LP RP
    */
    debug_print(root, DEBUGMODE);
    astNode *node0 = get_child(root, 0); // ID
    astNode *node2 = get_child(root, 2); // VarList/RP
    
    FunctionTable *newFunc = (FunctionTable *)malloc(sizeof(FunctionTable));
    Type *newType = (Type*)malloc(sizeof(Type));
    newType->kind = FUNCTION;
    newType->u.function.returntype = type;

    /* get function varlist */
    if (strcmp(node2->name, "RP")==0){ // FunDec -> ID LP RP 
        newType->u.function.paraNum = 0;
        newType->u.function.params = NULL;
    } else { // FunDec -> ID LP VarList RP 
        FieldList *field = VarList(node2);
        int paraNum = get_params_num(field);
        newType->u.function.paraNum = paraNum;
        newType->u.function.params = field;
    }

    /* 对每次的定义/声明，做如下的检查：
     1. pre声明，cur声明：检查是否一致，不一致报错，一致无操作
     2. pre声明，cur定义：检查是否一致，不一致报错，一致将其修改为已定义
     3. pre定义，cur声明：定义后声明报错
     4. pre定义，cur定义：重复定义报错
     5. pre没有，cur声明：插入isDefined
     6. pre没有，cur定义：插入isDefined
    */
    char *funcName = node0->value;
    FunctionTable *preFunc = get_functiontable_node(functionTable, funcName);
    if (preFunc == NULL) { // case 5 6
        insert_functiontable_node(functionTable, funcName, node0->lineno, isDefined, newType);
    } 
    else if (preFunc->isdef == 0) {
        if (isDefined == 0) {// case 1
            if (check_type_equal(newType, preFunc->type)==FALSE) {
                semantic_error(Conflict_Decordef_Funcion, root->lineno, node0->value);
            }
            // do nothing
        } else { // case 2
            if (check_type_equal(newType, preFunc->type)==FALSE) {
                semantic_error(Conflict_Decordef_Funcion, root->lineno, node0->value);
            } else {
                preFunc->isdef = 1;
            }
        }
    }
    else if (preFunc->isdef == 1) {// 直接报重定义就行
        semantic_error(Redefined_Function, root->lineno, node0->value);
    }

}

/* Check By myself */
FieldList *VarList(astNode *root){
    /*
    VarList: ParamDec COMMA VarList 
    | ParamDec   
    */
    debug_print(root, DEBUGMODE);
    
    astNode *node0 = get_child(root, 0);
    astNode *node2 = get_child(root, 2);
    /* 根据文法，VarList只用于形参表，形参表的第一个形参不可能重复定义 */
    FieldList *field = ParamDec(node0);
    if (node2 != NULL) {
        /* f代替field迭代 */
        FieldList *f = field;
        while (node2 != NULL && strcmp(node2->name, "VarList")==0){
            node0 = get_child(node2, 0);
            node2 = get_child(node2, 2);
            if (check_var_redefined(node0->value)){
                semantic_error(Redefined_Variable_Name, node0->lineno, node0->value);
            }
            f->nextFieldList = ParamDec(node0);
            f = f->nextFieldList;
        }
        f->nextFieldList = NULL;
    }
    insert_valid_fieldlist(field);
    return field;
}

/* Check By myself */
FieldList *ParamDec(astNode *root){
    /*
    ParamDec -> Specifier VarDec
    */
    debug_print(root, DEBUGMODE);

    astNode *node0 = get_child(root, 0);
    astNode *node1 = get_child(root, 1);
    if (node0 != NULL && node1 != NULL)
    {
        Type *type = Specifier(node0);
        return VarDec(node1, type);
    }
    return NULL;
}

/* Check By myself */
void CompSt(astNode *root, Type *type){
    /*
    CompSt -> LC DefList StmtList RC
    */
    debug_print(root, DEBUGMODE);

    astNode *node1 = get_child(root, 1);
    astNode *node2 = get_child(root, 2);

    if (node1 == NULL && node2 == NULL) { //CompSt -> LC  RC
        return;
    }
    else if (node1 == NULL && node2 != NULL) { //DefList -> empty
        StmtList(node2, type);
    }
    else if (node1 != NULL && node2 == NULL) {
        DefList(node1);
    }
    else if (strcmp(node1->name, "DefList")==0) {
        DefList(node1);
        if (strcmp(node2->name, "StmtList")==0)
            StmtList(node2, type);
    }
}   

/* Check By myself */
void StmtList(astNode *root, Type *type){
    /*
    StmtList -> Stmt StmtList
    | (empty)
    */
    debug_print(root, DEBUGMODE);

    astNode *node0 = get_child(root, 0);
    astNode *node1 = get_child(root, 1);
    if (node0 != NULL)
    {
        Stmt(node0, type);
        if (node1 != NULL)
            StmtList(node1, type);
    }
}

/* Check By myself */
void Stmt(astNode *root, Type *type){
    /*
    Stmt -> Exp SEMI
    | CompSt
    | RETURN Exp SEMI
    | IF LP Exp RP Stmt
    | IF LP Exp RP Stmt ELSE Stmt
    | WHILE LP Exp RP Stmt
    */
    debug_print(root, DEBUGMODE);
    astNode *node0 = get_child(root, 0);

    Type *INTtype = (Type*)malloc(sizeof(Type));
    INTtype->kind = BASIC;
    INTtype->u.basic = 0;

    if (strcmp(node0->name, "Exp")==0) { // Stmt -> Exp SEMI
        Exp(node0);
    }
    else if (strcmp(node0->name, "CompSt")==0) {
        scope_begin();
        CompSt(node0, type);
        scope_end();
    }
    else if (strcmp(node0->name, "RETURN")==0) {
        astNode *node1 = get_child(root, 1);
        Type *returnType = Exp(node1);

        /* 检查返回类型 */
        if (check_type_equal(returnType, type) == FALSE && returnType != NULL) {
            semantic_error(Return_Type_Dismatch, root->lineno, NULL);
        }
    }
    else if (strcmp(node0->name, "WHILE")==0) {
        astNode *node2 = get_child(root, 2);
        astNode *node4 = get_child(root, 4);
        /* while条件语句类型判断 */
        Type *whileType = Exp(node2);
        if (check_type_equal(whileType, INTtype)==FALSE) {
            semantic_error(Operand_Type_Dismatch, root->lineno, node2->value);
        }
        Stmt(node4, type);
    }
    else if (strcmp(node0->name, "IF")==0) {
        astNode *node2 = get_child(root, 2); // Exp
        astNode *node4 = get_child(root, 4); // IF Stmt
        astNode *node5 = get_child(root, 5); // ELSE
        /* condition exp check */
        Type *ifType = Exp(node2);
        if (check_type_equal(ifType, INTtype)==FALSE) {
            semantic_error(Operand_Type_Dismatch, root->lineno, node2->value);
        }
        /* execute if stmt */
        assert(node4 != NULL);
        Stmt(node4, type);
        /* execute else stmt */
        if (node5 != NULL) {
            astNode *node6 = get_child(root, 6); // ELSE Stmt
            assert(node6 != NULL);
            Stmt(node6, type);
        } 
    }
}

/* Check By myself */
void DefList(astNode *root){
    /*
    DefList -> Def DefList
    | (empty)
    */
    debug_print(root, DEBUGMODE);

    astNode *node0 = get_child(root, 0);
    astNode *node1 = get_child(root, 1);
    if (node0 != NULL)
    {
        Def(node0);
        if (node1 != NULL)
            DefList(node1);
    }
}

/* Check By myself */
void Def(astNode *root){
    /*
    Def -> Specifier DecList SEMI
    */
    debug_print(root, DEBUGMODE);

    astNode *node0 = get_child(root, 0);
    astNode *node1 = get_child(root, 1);
    if (node0 != NULL && node1 != NULL)
    {
        Type *type = Specifier(node0);
        DecList(node1, type);
    }
}

/* Check By myself */
void DecList(astNode *root, Type *type){
    /*
    DecList -> Dec
    | Dec COMMA DecList
    */
    debug_print(root, DEBUGMODE);

    astNode *node0 = get_child(root, 0);
    astNode *node1 = get_child(root, 1);
    astNode *node2 = get_child(root, 2);
    Dec(node0, type);

    if (node1 != NULL && node2 != NULL) // DecList -> Dec COMMA DecList
    {
        DecList(node2, type);
    }
}

/* Check By myself */
void Dec(astNode *root, Type *type){
    /*
    Dec -> VarDec
    | VarDec ASSIGNOP Exp
    */
    debug_print(root, DEBUGMODE);
    astNode *node0 = get_child(root, 0);
    astNode *node1 = get_child(root, 1);
    astNode *node2 = get_child(root, 2);
    FieldList *varField = VarDec(node0, type);
    int varType = (type->kind==STRUCTURE) ? 1 : 0;
    int check_res = check_var_struct_redefined(varField->name, varType);
    if (node1 == NULL) { // Dec -> VarDec
        /* 检查变量重定义 */
        if (check_res != 0) {// 没问题，直接插入
            semantic_error(Redefined_Variable_Name, root->lineno, varField->name);  
        } 
        /* 即使重复，也要插入，这样可以get最近声明的变量 */
        insert_symboltable_node(symbolTable, scopeStack, create_symboltable_node(varField->type, varField->name, varType));
    } else { // VarDec ASSIGNOP Exp
        /* 检查变量重定义 */
        if (check_res != 0) {// 尽管设了很多错误类型，但只要不是0就有错
            semantic_error(Redefined_Variable_Name, root->lineno, varField->name);
            insert_symboltable_node(symbolTable, scopeStack, create_symboltable_node(varField->type, varField->name, varType));
        } else { // 没问题，插入
            insert_symboltable_node(symbolTable, scopeStack, create_symboltable_node(varField->type, varField->name, varType));
            /* 检查赋值错误 */
            Type *expType = Exp(node2);
            if (check_type_equal(expType, varField->type)==FALSE){
                semantic_error(AssignOP_Type_Dismatch, root->lineno, NULL);
            }
        }
    }
}

Type *Exp(astNode *root){
    /*
    Exp -> Exp ASSIGNOP Exp
    | Exp AND Exp
    | Exp OR Exp
    | Exp RELOP Exp
    | Exp PLUS Exp
    | Exp MINUS Exp
    | Exp STAR Exp
    | Exp DIV Exp
    | Exp LB Exp RB
    | Exp DOT ID;

    | ID
    | INT
    | FLOAT

    | NOT Exp
    | ID LP Args RP
    | ID LP RP
    | LP Exp RP
    | MINUS Exp
    */
    debug_print(root, DEBUGMODE);

    if (root == NULL)
        return NULL;
    astNode *node0 = get_child(root, 0);
    astNode *node1 = get_child(root, 1);
    astNode *node2 = get_child(root, 2);
    astNode *node3 = get_child(root, 3);

    if (strcmp(node0->name, "Exp")==0)
    {
        if (strcmp(node1->name, "ASSIGNOP")==0) // Exp -> Exp ASSIGNOP Exp
        {
            astNode *node00 = get_child(node0, 0);
            astNode *node1 = get_child(node0, 1);
            astNode *node02 = get_child(node0, 2);
            astNode *node03 = get_child(node0, 3);
            if (node1 == NULL && !strcmp(node00->name, "ID")==0)
            {
                semantic_error(Leftside_Rvalue_Error, root->lineno, NULL);
                return NULL;
            }
            if (node1 != NULL)
            {
                if (node02 != NULL)
                {
                    if (node03 == NULL)
                    {
                        if (!strcmp(node00->name, "Exp")==0 || !strcmp(node1->name, "DOT")==0 || !strcmp(node02->name, "ID")==0)
                        {
                            semantic_error(Leftside_Rvalue_Error, root->lineno, NULL);
                            return NULL;
                        }
                    }
                    else if (!strcmp(node00->name, "Exp")==0 || !strcmp(node1->name, "LB")==0 || !strcmp(node02->name, "Exp")==0 || !strcmp(node03->name, "RB")==0)
                    {
                        semantic_error(Leftside_Rvalue_Error, root->lineno, NULL);
                        return NULL;
                    }
                }
                else
                {
                    semantic_error(Leftside_Rvalue_Error, root->lineno, NULL);
                    return NULL;
                }
            }
            Type *type1 = Exp(node0);
            Type *type2 = Exp(node2);
            if (check_type_equal(type1, type2) == FALSE)
            {
                semantic_error(AssignOP_Type_Dismatch, root->lineno, NULL);
                return NULL;
            }
            return type1;
        }
        else if (strcmp(node1->name, "AND")==0 || strcmp(node1->name, "OR")==0 || strcmp(node1->name, "RELOP")==0 || strcmp(node1->name, "PLUS")==0 || strcmp(node1->name, "MINUS")==0 || strcmp(node1->name, "STAR")==0 || strcmp(node1->name, "DIV")==0)
        {
            Type *type1 = Exp(node0);
            Type *type2 = Exp(node2);
            if (strcmp(node1->name, "AND")==0 || strcmp(node1->name, "OR")==0 || strcmp(node1->name, "RELOP")==0) {
                /* 逻辑运算 */
                Type *INTtype = (Type*)malloc(sizeof(Type));
                INTtype->kind = BASIC;
                INTtype->u.basic = 0;
                if (type1 == NULL || type2 == NULL) {
                    return INTtype;
                }
                else if (type1->u.basic == 1) {
                    semantic_error(Operand_Type_Dismatch, root->lineno, node0->value);
                    return INTtype;
                }
                else if (type2->u.basic == 1) {
                    semantic_error(Operand_Type_Dismatch, root->lineno, node2->value);
                    return INTtype;
                }
                else if (type1->u.basic==0 && type2->u.basic==0) {
                    return type1;
                }
                else {
                    semantic_error(Operand_Type_Dismatch, root->lineno, root->value);
                    return INTtype;
                }
            } else {
                /* 算数运算 */
                /* 处理空 */
                if (type1 == NULL && type2 != NULL) {
                    return type2;
                }
                else if (type1 != NULL && type2 == NULL) {
                    return type1;
                }
                else if (type1 == NULL && type2 == NULL) {
                    return NULL;
                }
                if (type1->u.basic == type2->u.basic) {
                    return type1;
                } else {
                    semantic_error(Operand_Type_Dismatch, root->lineno, root->value);
                    return type1;
                }
            }
        }
        else if (strcmp(node1->name, "LB")==0) // Exp -> Exp LB Exp RB
        {   
            Type *type1 = Exp(node0);
            Type *type2 = Exp(node2);
            bool valid_access = TRUE;
            /* 处理空 */
            if (type1 == NULL || type2 == NULL) {
                return NULL;
            }
            /* 继续处理 */
            if (type1->kind != ARRAY) /* 非数组类型使用[] */
            {
                semantic_error(Operate_Others_As_Array, root->lineno, NULL);
                valid_access = FALSE;
            }
            if (!((type2->kind == BASIC || type2->kind == FUNCTION) && type2->u.basic == 0))
            {/* 使用浮点访问数组 */
                semantic_error(Array_Float_Index, root->lineno, NULL);
                valid_access = FALSE;
            }
            /* 若合法访问，应返回数组的元素；
               若非法访问，这里选择返回原本的类型 */
            return valid_access? type1->u.array.elem: type1;
        }
        else if (strcmp(node1->name, "DOT")==0) // Exp -> Exp DOT ID
        {
            Type *type1 = Exp(node0);
            if (type1 == NULL)
                return NULL;
            if (type1->kind != STRUCTURE)
            {
                semantic_error(Operate_Others_As_Struct, root->lineno, NULL);
                return NULL;
            }
            FieldList *field = type1->u.structure.structures;
            while (field != NULL)
            {
                if (strcmp(field->name, node2->value)==0)
                    return field->type;
                field = field->nextFieldList;
            }
            semantic_error(Undefined_Field, root->lineno, node2->value);
            return NULL;
        }
    }
    else if (node1 == NULL)
    {
        if (strcmp(node0->name, "ID")==0)
        {
            SymbolTableNode *result = get_symboltable_node(symbolTable, node0->value);
            if (result == NULL)
            {
                semantic_error(Undefined_Variable, root->lineno, node0->value);
                return NULL;
            }
            else
            {
                return result->type;
            }
        }
        else if (strcmp(node0->name, "INT")==0)
        {

            Type *result = NULL;
            result = (Type*)(malloc(sizeof(Type)));
            result->kind = BASIC;
            result->u.basic = 0;
            return result;
        }
        else if (strcmp(node0->name, "FLOAT")==0)
        {

            Type *result = NULL;
            result = (Type*)(malloc(sizeof(Type)));
            result->kind = BASIC;
            result->u.basic = 1;
            return result;
        }
    }
    else
    {
        if (strcmp(node0->name, "LP")==0) // LP Exp RP
        {
            return Exp(node1);
        }
        else if (strcmp(node0->name, "MINUS")==0) // MINUS Exp
        {
            Type *type = Exp(node1);
            if (type == NULL)
                return NULL;
            if (type->kind != BASIC)
            {
                semantic_error(Operand_Type_Dismatch, root->lineno, root->value);
                return NULL;
            }
            return type;
        }
        else if (strcmp(node0->name, "NOT")==0) // NOT Exp
        {
            Type *type = Exp(node1);
            if (type == NULL)
                return NULL;
            if (type->kind == BASIC && type->u.basic == 0)
                return type;
            semantic_error(Operand_Type_Dismatch, root->lineno, root->value);
            return NULL;
        }
        else if (strcmp(node0->name, "ID")==0)
        {
            FunctionTable *func = get_functiontable_node(functionTable, node0->value);
            SymbolTableNode *var = get_symboltable_node(symbolTable, node0->value);
            
            /* 检查函数定义，以及变量错误 */
            if (var != NULL)// 对非函数使用了()
            {
                semantic_error(Operate_Basic_As_Func, root->lineno, node0->value);
                return NULL;
            }
            if (func == NULL) // 函数未定义
            {
                semantic_error(Undefined_Function, root->lineno, node0->value);
                return NULL;
            }

            /* 检查完毕可以访问，无论是否正确访问，都正常返回 */
            Type *type = func->type;
            Type *returntype = type->u.function.returntype;
            if (strcmp(node2->name, "Args")==0) // ID LP Args RP
            {   
                if (type->u.function.params == NULL) {   //Args是非空的
                    semantic_error(Func_Call_Parameter_Dismatch, root->lineno, node0->value);
                } else {
                    FieldList *argsField = Args(node2);
                    FieldList *paraField = type->u.function.params;
                    if (check_fieldlist_equal(paraField, argsField)==FALSE){
                        semantic_error(Func_Call_Parameter_Dismatch, root->lineno, node0->value);
                    }
                }
            }
            else if (strcmp(node2->name, "RP")==0) {// ID LP RP
                if (type->u.function.params != NULL) {
                    semantic_error(Func_Call_Parameter_Dismatch, root->lineno, node0->value);
                }
            }
            return returntype;// 错误调用也正常返回
        }
    }
    return NULL;
}

FieldList *Args(astNode *root){
    /*
    Args -> Exp COMMA Args
    | Exp;
    */
    debug_print(root, DEBUGMODE);
    astNode *node0 = get_child(root, 0);
    astNode *node2 = get_child(root, 2);
    
    FieldList *args = (FieldList*)malloc(sizeof(FieldList));
    args->type = Exp(node0); // 可能是NULL
    args->name = NULL;// 不需要名字
    args->nextFieldList = NULL;

    if (node2 != NULL) // Args -> Exp COMMA Args
    {
        args->nextFieldList = Args(node2);
    }

    return args;
}

void check_undef_func()
{
    FunctionTable *curFunc = functionTable;
    while (curFunc != NULL) {
        if (curFunc->isdef == 0) {
            semantic_error(Undefined_Function_But_Declaration, curFunc->lineNo, curFunc->name);
        }
        curFunc = curFunc->nextFun;
    }
}

void semantic_error(enum SemanticError errortype, int line, char *message){
    printf("Error type %d at Line %d: ", errortype + 1, line);
    switch (errortype)
    {
    case Undefined_Variable:
    {
        printf("Variable \"%s\" is unfined.\n", message);
    }
    break;
    case Undefined_Function:
    {
        printf("Function \"%s\" is undefined.\n", message);
    }
    break;
    case Redefined_Variable_Name:
    {
        printf("Variable \"%s\" is redefined.\n", message);
    }
    break;
    case Redefined_Function:
    {
        printf("Function \"%s\" is redefined.\n", message);
    }
    break;
    case AssignOP_Type_Dismatch:
    {
        printf("Type mismatch of assign \"=\".\n");
    }
    break;
    case Leftside_Rvalue_Error:
    {
        printf("A right value expression can't be assigned.\n");
    }
    break;
    case Operand_Type_Dismatch:
    {
        printf("Illegal expression because \"%s\" has a wrong type.\n", message);
    }
    break;
    case Return_Type_Dismatch:
    {
        printf("Wrong return Type.\n");
    }
    break;
    case Func_Call_Parameter_Dismatch:
    {
        printf("Parameters mismatch when calling the function \"%s\".\n", message);
    }
    break;
    case Operate_Others_As_Array:
    {
        printf("Illegal accessing a non-array var with [].\n");
    }
    break;
    case Operate_Basic_As_Func:
    {
        printf("Illegal calling a non-function var with ().\n");
    }
    break;
    case Array_Float_Index:
    {
        printf("Use float as an array index.\n");
    }
    break;
    case Operate_Others_As_Struct:
    {
        printf("Illegal accessing a non-struct var with \".\".\n");
    }
    break;
    case Undefined_Field:
    {
        printf("Undefined structure field \"%s\".\n", message);
    }

    break;
    case Redefined_Field:
    {
        printf("Redefined or Illegal initialized structure field \"%s\".\n", message);
    }
    break;
    case Redefined_Field_Name:
    {
        printf("Redefined structure \"%s\".\n", message);
    }
    break;
    case Undefined_Struct:
    {
        printf("Undefined structure \"%s\".\n", message);
    }
    break;
    case Undefined_Function_But_Declaration:
    {
        printf("Function \"%s\" haven't been defined but declared.\n", message);
    }
    break;
    case Conflict_Decordef_Funcion:
    {
        printf("Function's statement \"%s\" conflit with defination or other statements.\n", message);
    }
    break;

    default:
    {
        printf("Other Mistakes, content is :%s\n", message);
    }
    break;
    }
}

/* Packaged Data Structure Method */
void scope_begin(){
    push_scope(scopeStack);
}

void scope_end(){
    pop_scope(symbolTable, scopeStack);
}

void insert_valid_fieldlist(FieldList *field){
    FieldList *curField = field;
    int isStruct = 0;
    while(curField != NULL){
        /* a inserted field list can be only a var or a struct */
        if (curField->type->kind == STRUCTURE){
            isStruct = 1;
        } else {
            isStruct = 0;
        }
        SymbolTableNode *symbol = create_symboltable_node(curField->type, curField->name, isStruct);// kind = 0, a var
        insert_symboltable_node(symbolTable, scopeStack, symbol);
        curField = curField->nextFieldList;
    }
    
}

SymbolTableNode *get_scope_first_node(){
    return scopeStack->top->ScopeHeadNode;
}

int get_params_num(FieldList *params){
    if (params == NULL) {
        return 0;
    } else {
        int num = 0;
        FieldList *curParam = params;
        while (curParam != NULL){
            num++;
            curParam = curParam->nextFieldList;
        }
        return num;
    }
    assert(0);
}


/* check variable is redefined */
bool check_var_redefined(char *name){
    if (get_scope_node(scopeStack, name)==NULL){// 应该在同一个scope内找重复定义
        return FALSE;
    } else {
        return TRUE;
    }
}

bool check_struct_redefined(char *name){
    if (get_structuretable_node(structTable, name)==NULL){
        return FALSE;
    } else {
        return TRUE;
    }
}

bool check_type_equal(Type *a, Type *b){
    if (a == b){
        return TRUE;
    }

    if (a == NULL || b == NULL || a->kind != b->kind){
        return FALSE;
    }
    if (SHOW_PROCESS == 1) {
        printf("\n");
        printf("Current A type is:\n");
        type_print(a);
        printf("\n");
        printf("Current B type is:\n");
        type_print(b);
        printf("\n");
    }
    switch (a->kind){
        case BASIC:{
            return a->u.basic == b->u.basic;
        } break;
        case ARRAY: {//数组，弱相等（维度相等），类型已经在kind检查过
            Type *pt1 = a, *pt2 = b;
            int d1 = 0, d2 = 0;
            while (pt1 != NULL) {
                d1 += 1;
                pt1 = pt1->u.array.elem;
                if (pt1->kind != ARRAY)
                    break;
            }
            while (pt2 != NULL) {
                d2 += 1;
                pt2 = pt2->u.array.elem;
                if (pt2->kind != ARRAY)
                    break;
            }
            return d1 == d2;
        } break;
        case STRUCTURE: {//结构体
            
            FieldList *s1 = a->u.structure.structures;
            FieldList *s2 = b->u.structure.structures;
            while (s1 != NULL && s2 != NULL) {
                if (s1->type->kind != s2->type->kind) {
                    return FALSE;
                } else {
                    if (s1->type->kind == ARRAY) { //数组强相等
                        if (array_strong_equal(s1->type, s2->type) == FALSE)
                            return FALSE;
                    } else {
                        if (check_type_equal(s1->type, s2->type) == FALSE) {
                            return FALSE;
                        }
                    }
                }
                s1 = s1->nextFieldList;
                s2 = s2->nextFieldList;
            }
            if (s1 == NULL && s2 == NULL) {
                return TRUE;
            }
            return FALSE;
        } break;
        case FUNCTION:
        {
            if (a->u.function.paraNum != b->u.function.paraNum || 
                check_type_equal(a->u.function.returntype, b->u.function.returntype) == FALSE) {
                return FALSE;
            }
            FieldList *p1 = a->u.function.params;
            FieldList *p2 = b->u.function.params;
            for (int i = 0; i < a->u.function.paraNum; i++) {
                if (check_type_equal(p1->type, p2->type) == FALSE)
                    return FALSE;
                p1 = p1->nextFieldList;
                p2 = p2->nextFieldList;
            }
            return TRUE;
        } break;
        default: {
            printf("No such type!\n");
            return FALSE;
        } break;
    }
}

bool check_fieldlist_equal(FieldList *a, FieldList* b){
    if (a == NULL & b != NULL || a != NULL & b == NULL) {
        return FALSE;
    }
    if (a == NULL & b == NULL) {
        return TRUE;
    }
    FieldList *curA = a;
    FieldList *curB = b;
    
    while (curA != NULL && curB != NULL) {
        // 比较字段的类型是否相等
        if (check_type_equal(curA->type, curB->type) == FALSE) {
            return FALSE;
        }
        curA = curA->nextFieldList;
        curB = curB->nextFieldList;
    }
    if (curA != NULL || curB != NULL) {
        return FALSE;
    }
    return TRUE; 
}
bool array_strong_equal(Type *a, Type *b)
{
    if (a->u.array.size != b->u.array.size || a->u.array.elem->kind != b->u.array.elem->kind) {
        return FALSE;
    }

    if (a->u.array.elem->kind == ARRAY) {
        return array_strong_equal(a->u.array.elem, b->u.array.elem);
    } else {
        return check_type_equal(a->u.array.elem, b->u.array.elem);
    }
    return FALSE;
}
/* 假设7要求变量名与结构体的域名不重复，但此处仅考虑变量名与结构名重复的报错 */
int check_var_struct_redefined(char *name, int curType) {// curType: 1代表是结构体名，0代表是变量名
    /* 检查变量或结构本身有没有重定义 */
    if (SHOW_PROCESS == 1){
        scope_print();
    }

    if (curType == 0) {
        if (check_var_redefined(name)){
            return 3;//变量定义冲突
        }
    }
    if (curType == 1) {
        if (check_struct_redefined(name)){
            return 4;//结构体定义冲突
        }
    }
    SymbolTableNode *var = get_scope_node(scopeStack, name);
    StructureTable *structure = get_structuretable_node(structTable, name);
    if (SHOW_PROCESS == 1){
        printf("\n");
        printf("Checking %s \'%s\'\n", curType?"struct":"var", name);
        printf("Search symboltable, %sexisted \n", var==NULL?"don't":"");
        printf("Search structtable, %sexisted \n", structure==NULL?"don't":"");
        printf("\n");
    }
    
    if (structure == NULL && var == NULL) {
        /* 都不存在，没有重名 */
        return 0;
    }
    else if (structure != NULL && var == NULL && curType == 0) {
        /* 对于var, 存在重名的struct */
        return 1;
    }
    else if (structure == NULL && var != NULL && curType == 1) {
        /* 对于struct，存在重名的var */
        return 2;
    }
}

/* check field list has redefined */
bool check_fieldlist_redefined(FieldList *field, char *name) {
    if (field == NULL) {
        return FALSE;
    }
    FieldList *curField = field;
    while(curField != NULL){
        if (strcmp(curField->name,name)==0) {
            return TRUE;
        }
        curField = curField->nextFieldList;
    }
    return FALSE;
}