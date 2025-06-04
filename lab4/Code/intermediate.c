#include "intermediate.h"
#include "debugger.h"
// has been defined in symboltable.h 
// #define bool int
// #define true 1
// #define false 0
#define DEBUGMODE 0
/* Global Var */
InterCodeList *headCode = NULL;
InterCodeList *tailCode = NULL;
snapshot *curSnapshot = NULL; //分析到当前位置的符号表+作用域的快照，类似于tuple封装两个数据，方便对Table和Stack操作
int varNo   = 1; //变量编号
int tempNo  = 1; //临时变量编号
int labelNo = 1; //标号编号

/* result from semantic.c */
extern HashTable *symbolTable;
extern ScopeStack *scopeStack;
extern FunctionTable *functionTable;
extern StructureTable *structTable;

/* Done */
void translate_Program(astNode *root, FILE *file){
    /*
    Program -> ExfDefList
    */
    /* init */
    print_procedure(root, DEBUGMODE);
    headCode = init_intercodelist();
    tailCode = headCode;
    // 语义分析的结果，实际上cur指向了最内部的作用域，由于实验三简化了作用域的要求，因此只有一个全局作用域
    // TODO：在翻译过程中需要根据作用域变化来enter和exit，以恢复该时期的作用域和符号表信息，完成TODO前不需要知道作用域的信息
    curSnapshot = (snapshot*)malloc(sizeof(snapshot));
    curSnapshot->curStack = scopeStack;
    curSnapshot->curTable = symbolTable;
    enter_innermost_scope(curSnapshot);
    // 将结构体中的所有域名计算偏移并加入到符号表（约束保证域名和变量不重复）
    insert_struct(structTable);
    // print_struct_table(structTable, DEBUGMODE);
    // print_symbol_table(curSnapshot->curTable, DEBUGMODE);
    /* translate begin */
    translate_ExtDefList(get_child(root, 0));
    /* translate end */

    // print_intercode(headCode, file);
}
/* Done */
void translate_ExtDefList(astNode *root){
    /*
    ExfDefList -> ExfDef ExfDefList
    | (empty)
    */
    print_procedure(root, DEBUGMODE);
    astNode *ExtDef = get_child(root, 0);
    astNode *ExtDefList = get_child(root, 1);
    if (ExtDef != NULL && ExtDefList != NULL){
        translate_ExtDef(ExtDef);
        translate_ExtDefList(ExtDefList);
    }
    if (ExtDef != NULL && ExtDefList == NULL){ //empty
        
        translate_ExtDef(ExtDef);
    }
}
/* Done */
void translate_ExtDef(astNode *root){
    /*
    ExtDef -> Specifier ExtDecList SEMI
    | Specifier SEMI
    | Specifier FunDec CompSt
    | Specifier FunDec SEMI
    */
    print_procedure(root, DEBUGMODE);
    
    astNode *node1 = get_child(root, 1);
    astNode *node2 = get_child(root, 2);
    
    if (strcmp(node1->name, "ExtDecList")==0) {
        translate_ExtDecList(node1);
    }
    else if (strcmp(node1->name, "FunDec")==0) {
        
        if (strcmp(node2->name, "CompSt") == 0) {
            
            enter_scope(curSnapshot);
            translate_FunDec(node1);
            translate_CompSt(node2);
            exit_scope(curSnapshot);
        } else {
            /* Specifier FunDec SEMI */
            // 声明不需要翻译，但是由于声明会对作用域以及符号表产生影响，因此需要enter和exit以保证能正确的恢复符号表
            enter_scope(curSnapshot);
            exit_scope(curSnapshot);
        }
    }
}
/* Done */
void translate_ExtDecList(astNode *root){
    /*
    ExtDecList -> VarDec
    | VarDec COMMA ExtDecList
     */
    print_procedure(root, DEBUGMODE);
    astNode *VarDec = get_child(root, 0);
    astNode *ExtDecList = get_child(root, 2);

    if (VarDec != NULL) {
        translate_VarDec(VarDec);
        if (ExtDecList != NULL) {
            translate_ExtDecList(ExtDecList);
        }
    }
}
/* Done */
Operand *translate_VarDec(astNode *root){
    /*
    VarDec -> ID
    | VarDec LB INT RB
     */
    print_procedure(root, DEBUGMODE);
    Operand *ret = NULL;
    if (root == NULL) {
        return NULL;
    }

    //代码有一些重复，本质就是得到ID后查表，TODO：简化
    astNode *node0 = get_child(root, 0);
    if (strcmp(node0->name, "ID") == 0) { // VarDec -> ID
        SymbolTableNode *IDnode = get_symboltable_node(curSnapshot->curTable, node0->value); 
        ret = create_oprand(VARIABLE_OP, VAL, node0->value);
        IDnode->var_no = ret->var_no;
        IDnode->isAddr = ret->type;
        if (get_size(IDnode->type) != 4) {
            Operand *op = create_oprand(CONSTANT_OP, VAL, get_size(IDnode->type));
            generate_code(DEC_INTERCODE, ret, op);
        }
    } else { // VarDec -> VarDec LB INT RB
        astNode *curID = get_child(node0, 0);
        while (strcmp(curID->name, "ID")!=0) {/* 一直展开到ID */
            curID = get_child(curID, 0);
        }
        SymbolTableNode *IDnode = get_symboltable_node(curSnapshot->curTable, curID->value);
        ret = create_oprand(VARIABLE_OP, VAL, curID->value);
        IDnode->var_no = ret->var_no;
        IDnode->isAddr = ret->type;
        Operand *op = create_oprand(CONSTANT_OP, VAL, get_size(IDnode->type));
        generate_code(DEC_INTERCODE, ret, op);
    }
    return ret;
}
/* Done */
void translate_FunDec(astNode *root){
    /*
    FunDec -> ID LP VarList RP
    | ID LP RP
     */
    print_procedure(root, DEBUGMODE);
    astNode *IDnode = get_child(root, 0);
    Operand *op = create_oprand(FUNCTION_OP, VAL, IDnode->value);
    generate_code(FUNC_INTERCODE, op);

    FunctionTable *IDsymbol = get_functiontable_node(functionTable, op->funcName);
    if (IDsymbol == NULL) {
        return;
    }
    if (IDsymbol->type->u.function.paraNum != 0) {//额外声明函数形参
        FieldList *parameters = IDsymbol->type->u.function.params;
        while (parameters != NULL) {
            Operand *tmpOp = NULL;
            if (parameters->type->kind == ARRAY || parameters->type->kind == STRUCTURE) {
                tmpOp = create_oprand(VARIABLE_OP, ADDRESS, parameters->name);
            } else {
                tmpOp = create_oprand(VARIABLE_OP, VAL, parameters->name);
            }
            SymbolTableNode *query4param = get_symboltable_node(curSnapshot->curTable, parameters->name);
            if (query4param == NULL) {
                return;
            }
            query4param->var_no = tmpOp->var_no;
            query4param->isAddr = tmpOp->type;
            tmpOp->type = VARIABLE_OP;
            generate_code(PARAM_INTERCODE, tmpOp);
            parameters = parameters->nextFieldList;
        }
    }
}
/* Done */
void translate_VarList(astNode *root){
    /*
    VarList -> ParamDec COMMA VarList
    | ParamDec
    */
    print_procedure(root, DEBUGMODE);
    astNode *ParamDec = get_child(root, 0);
    translate_ParamDec(ParamDec);
    astNode *VarList = get_child(root, 2);
    if (VarList != NULL) {
        translate_VarList(VarList);
    }
}
/* Done */
void translate_CompSt(astNode *root){
    /*
    CompSt -> LC DefList StmtList RC
    */
    print_procedure(root, DEBUGMODE);
    astNode *node1 = get_child(root, 1);
    astNode *node2 = get_child(root, 2);
    if (node1 != NULL && node2 != NULL) {// LC DefList StmtList RC
        translate_DefList(node1);
        translate_StmtList(node2);
    } 
    else if (node1 == NULL && node2 != NULL) {// LC NULL StmtList RC
        translate_StmtList(node2);
    } 
    else if (node1 != NULL && node2 == NULL) {// LC DefList NULL RC
        translate_DefList(node1);
    }
    // LC NULL NULL RC
}
/* Done */
void translate_StmtList(astNode *root){
    /*
    StmtList -> Stmt StmtList
    | (empty)
     */
    print_procedure(root, DEBUGMODE);
    astNode *Stmt = get_child(root, 0);
    astNode *StmtList = get_child(root, 1);
    if (Stmt != NULL)
    {
        translate_Stmt(Stmt);
        if (StmtList != NULL)
        {
            translate_StmtList(StmtList);
        }
    }
}
/* Done */
void translate_Stmt(astNode *root){
    /*
    Stmt -> Exp SEMI
    | CompSt
    | RETURN_INTERCODE Exp SEMI
    | IF LP Exp RP Stmt
    | IF LP Exp RP Stmt ELSE Stmt
    | WHILE LP Exp RP Stmt
     */
    print_procedure(root, DEBUGMODE);
    astNode *node0 = get_child(root, 0);
    astNode *node1 = get_child(root, 1);
    astNode *node2 = get_child(root, 2);
    astNode *node3 = get_child(root, 3);
    astNode *node4 = get_child(root, 4);
    astNode *node5 = get_child(root, 5);
    astNode *node6 = get_child(root, 6);
    if (node0 != NULL) {
        if (strcmp(node0->name, "Exp") == 0) {//Exp SEMI
            translate_Exp(node0);
        }
        else if (strcmp(node0->name, "CompSt") == 0) {//CompSt
            translate_CompSt(node0);
        }
        else if (strcmp(node0->name, "RETURN") == 0) {//RETURN_INTERCODE Exp SEMI
            Operand *op = translate_Exp(node1);
            generate_code(RETURN_INTERCODE, op);
        }
        else if (strcmp(node0->name, "WHILE") == 0) {// WHILE LP Exp RP Stmt
            Operand *label1 = create_oprand(LABEL_OP, VAL);
            Operand *label2 = create_oprand(LABEL_OP, VAL);
            generate_code(LABEL_INTERCODE, label1);
            translate_Cond(node2, NULL, label2);
            translate_Stmt(node4);
            generate_code(GOTO_INTERCODE, label1);
            generate_code(LABEL_INTERCODE, label2);
        }
        else if (strcmp(node0->name, "IF") == 0) {
            Operand *label1 = create_oprand(LABEL_OP, VAL);
            translate_Cond(node2, NULL, label1);
            translate_Stmt(node4);
            if (node5 != NULL) {//IF LP Exp RP Stmt ELSE Stmt
                Operand *label2 = create_oprand(LABEL_OP, VAL);
                generate_code(GOTO_INTERCODE, label2);
                generate_code(LABEL_INTERCODE, label1);
                translate_Stmt(node6);
                generate_code(LABEL_INTERCODE, label2);
            } else {//IF LP Exp RP Stmt
                generate_code(LABEL_INTERCODE, label1);
            }
        }
    }
}
/* Done */
void translate_DefList(astNode *root){
    /*
    DefList -> Def DefList
    | (empty)
     */
    print_procedure(root, DEBUGMODE);
    astNode *Def = get_child(root, 0);
    astNode *DefList = get_child(root, 1);
    if (Def != NULL)
    {
        translate_Def(Def);
        if (DefList != NULL)
        {
            translate_DefList(DefList);
        }
    }
}
/* Done */
void translate_Def(astNode *root){
    /*
    Def -> Specifier DecList SEMI
     */
    print_procedure(root, DEBUGMODE);
    astNode *DecList = get_child(root, 1);
    if (DecList != NULL)
    {
        translate_DecList(DecList);
    }
}
/* Done */
void translate_DecList(astNode *root){
    /*
    DecList -> Dec
    | Dec COMMA DecList
     */
    print_procedure(root, DEBUGMODE);
    astNode *Dec = get_child(root, 0);
    astNode *DecList = get_child(root, 2);
    if (Dec != NULL) {
        translate_Dec(Dec);
        if (DecList != NULL) {
            translate_DecList(DecList);
        }
    }
}
/* Done */
void translate_Dec(astNode *root){
    /*
    Dec -> VarDec
    | VarDec ASSIGNOP Exp
     */
    print_procedure(root, DEBUGMODE);
    astNode *VarDec = get_child(root, 0);
    astNode *Exp = get_child(root, 2);
    if (Exp == NULL) {//只声明
        translate_VarDec(VarDec);
    } else { //声明+赋值
        Operand *op = translate_VarDec(VarDec);
        Operand *exp = translate_Exp(Exp);
        generate_code(ASSIGN_INTERCODE, op, exp);
    }
}
/* Done */
Operand *translate_Exp(astNode *root){
    /*
    Exp -> Exp ASSIGNOP Exp
    | Exp AND Exp
    | Exp OR Exp
    | Exp RELOP Exp
    | Exp PLUS Exp
    | Exp MINUS Exp
    | Exp STAR Exp
    | Exp DIV Exp
    | LP Exp RP
    | MINUS Exp
    | NOT Exp
    | ID LP Args RP
    | ID LP RP
    | Exp LB Exp RB
    | Exp DOT ID
    | ID
    | INT
    | FLOAT
     */
    print_procedure(root, DEBUGMODE);
    Operand *ret = NULL;
    astNode *node0 = get_child(root, 0);
    astNode *node1 = get_child(root, 1);
    astNode *node2 = get_child(root, 2);
    if (node0 == NULL) {
        return NULL;
    }

    if (strcmp(node0->name, "Exp") == 0) {
        if (strcmp(node1->name, "ASSIGNOP") == 0) { // Exp ASSIGNOP Exp
            Operand *op1 = translate_Exp(node0);
            Operand *op2 = translate_Exp(node2);

            int flag = 0;
            if (op1->varName != NULL && op2->varName != NULL) {
                SymbolTableNode *op1Node = get_symboltable_node(curSnapshot->curTable, op1->varName);
                SymbolTableNode *op2Node = get_symboltable_node(curSnapshot->curTable, op2->varName);
                if (op1Node->type->kind == ARRAY && op2Node->type->kind == ARRAY && op1->type == VAL && op2->type == VAL) {
                    flag = 1;
                }
            }

            if (flag == 1) {
                SymbolTableNode *op1Node = get_symboltable_node(curSnapshot->curTable, op1->varName);
                SymbolTableNode *op2Node = get_symboltable_node(curSnapshot->curTable, op2->varName);
                int depth1 = op1->depth;
                int depth2 = op2->depth;
                int size = get_size(op2Node->type);
                Operand *constOp = create_oprand(CONSTANT_OP, VAL, size);
                Operand *constOp4 = create_oprand(CONSTANT_OP, VAL, 4);
                Operand *cp1 = copy_operand(op1);
                Operand *cp2 = copy_operand(op2);
                if (cp1->kind == VARIABLE_OP) {
                    cp1->type = ADDRESS;
                }
                if (cp2->kind == VARIABLE_OP) {
                    cp2->type = ADDRESS;
                }

                Operand *tmpOp1 = create_oprand(TEMP_OP, VAL);
                generate_code(ASSIGN_INTERCODE, tmpOp1, cp1);
                Operand *tmpOp2 = create_oprand(TEMP_OP, VAL);
                generate_code(ASSIGN_INTERCODE, tmpOp2, cp2);

                Operand *varOp = create_oprand(VARIABLE_OP, ADDRESS, op2->varName);
                varOp->var_no = op2Node->var_no;
                Operand *endOp = create_oprand(TEMP_OP, VAL);
                generate_code(ADD_INTERCODE, endOp, varOp, constOp);

                Operand *labelOp1 = create_oprand(LABEL_OP, VAL);
                Operand *labelOp2 = create_oprand(LABEL_OP, VAL);
                generate_code(LABEL_INTERCODE, labelOp1);
                generate_code(LABEL_INTERCODE, tmpOp2, ">=", endOp, labelOp2);

                Operand *cptmpOp1 = copy_operand(tmpOp1);
                Operand *cptmpOp2 = copy_operand(tmpOp2);
                cptmpOp1->type = ADDRESS;
                cptmpOp2->type = ADDRESS;
                generate_code(ASSIGN_INTERCODE, cptmpOp1, cptmpOp2);
                generate_code(ADD_INTERCODE, tmpOp1, tmpOp1, constOp4);
                generate_code(ADD_INTERCODE, tmpOp2, tmpOp2, constOp4);
                generate_code(GOTO_INTERCODE, labelOp1);
                generate_code(LABEL_INTERCODE, labelOp2);
            }
            else if (op1 != NULL && op2 != NULL) {
                generate_code(ASSIGN_INTERCODE, op1, op2);
            }

            return op1;
        } 
        else if (strcmp(node1->name, "AND") == 0 || strcmp(node1->name, "OR") == 0 || strcmp(node1->name, "RELOP") == 0) {
            Operand *label1 = create_oprand(LABEL_OP, VAL);
            Operand *label2 = create_oprand(LABEL_OP, VAL);
            ret = create_oprand(TEMP_OP, VAL);
            Operand *newOp = create_oprand(CONSTANT_OP, VAL, 0);
            generate_code(ASSIGN_INTERCODE, ret, newOp);

            translate_Cond(root, label1, label2);
            generate_code(LABEL_INTERCODE, label1);

            Operand *oneOp = create_oprand(CONSTANT_OP, VAL, 1);
            generate_code(ASSIGN_INTERCODE, ret, oneOp);
            generate_code(LABEL_INTERCODE, label2);

            return ret;
        } 
        else if (strcmp(node1->name, "PLUS") == 0 || strcmp(node1->name, "MINUS") == 0 || strcmp(node1->name, "STAR") == 0 || strcmp(node1->name, "DIV") == 0) {
            int kind;
            if (strcmp(node1->name, "PLUS") == 0) {
                kind = ADD_INTERCODE;
            } else if (strcmp(node1->name, "MINUS") == 0) {
                kind = SUB_INTERCODE;
            } else if (strcmp(node1->name, "STAR") == 0) {
                kind = MUL_INTERCODE;
            } else {
                kind = DIV_INTERCODE;
            }

            ret = create_oprand(TEMP_OP, VAL);
            Operand *op1 = translate_Exp(node0);
            Operand *op2 = translate_Exp(node2);
            if (op1 != NULL && op2 != NULL) {
                generate_code(kind, ret, op1, op2);
            }
            return ret;
        } 
        else if (strcmp(node1->name, "LB") == 0) { // Exp LB Exp RB
            Operand *cpExp = copy_operand(translate_Exp(node0));
            int depth = cpExp->depth;
            SymbolTableNode *symbol = get_symboltable_node(curSnapshot->curTable, cpExp->varName);
            Type *tmpType = symbol->type;
            int count = 0;
            while (tmpType->kind == ARRAY) {
                count++;
                tmpType = tmpType->u.array.elem;
            }

            int size = get_size(tmpType);
            int *arraySize = (int*)malloc(sizeof(int) * (count+1));
            
            count = 0;
            tmpType = symbol->type;
            while (tmpType->kind == ARRAY) {
                
                arraySize[count] = tmpType->u.array.size;
                count++;
                tmpType = tmpType->u.array.elem;
            }
            int idx = 0;
            int tmpDepth = count - depth - 1;
            int offset = 1;
            while (tmpDepth) {
                offset *= arraySize[idx];
                idx++;
                tmpDepth--;
            }
            free(arraySize);
            offset = offset * size;
            
            Operand *ExpOp = translate_Exp(node2);

            Operand *tmpOp1 = create_oprand(TEMP_OP, VAL);
            Operand *constOp1 = create_oprand(CONSTANT_OP, VAL, offset);
            generate_code(MUL_INTERCODE, tmpOp1, ExpOp, constOp1);

            Operand *tmpOp2 = create_oprand(TEMP_OP, VAL);
            tmpOp2->varName = cpExp->varName;
            tmpOp2->depth = depth + 1;

            if (depth == 0 && cpExp->type == VAL) {
                cpExp->type = ADDRESS;
            } else {
                cpExp->type = VAL;
            }
            generate_code(ADD_INTERCODE, tmpOp2, cpExp, tmpOp1);

            ret = copy_operand(tmpOp2);
            if (tmpOp2->depth == count) {
                ret->type = ADDRESS;
            }
            return ret;
        }
        else if (strcmp(node1->name, "DOT") == 0) { // Exp DOT ID
            Operand *ExpOp = translate_Exp(node0);
            Operand *cpExpOp = copy_operand(ExpOp);
            SymbolTableNode *symbol = get_symboltable_node(curSnapshot->curTable, node2->value);
            
            int offset = symbol->offset;
            if (offset == 0) {
                Operand *tmpOp = create_oprand(TEMP_OP, VAL);
                if (cpExpOp->type == ADDRESS) {
                    cpExpOp->type = VAL;
                } else {
                    cpExpOp->type = ADDRESS;
                }
                generate_code(ASSIGN_INTERCODE, tmpOp, cpExpOp);
                ret = copy_operand(tmpOp);
                ret->type = ADDRESS;
                ret->varName = node2->value;
                return ret;
            } else {
                Operand  *constOp = create_oprand(CONSTANT_OP, VAL, offset);
                Operand *tmpOp = create_oprand(TEMP_OP, VAL);
                if (cpExpOp->type == ADDRESS) {
                    cpExpOp->type = VAL;
                } else {
                    cpExpOp->type = ADDRESS;
                }
                generate_code(ADD_INTERCODE, tmpOp, cpExpOp, constOp);
                ret = copy_operand(tmpOp);
                ret->type = ADDRESS;
                ret->varName = node2->value;
                return ret;
            }
        } 
    }
    else if (strcmp(node0->name, "LP") == 0) { // LP Exp RP
        ret = translate_Exp(node1);
        return ret;
    } 
    else if (strcmp(node0->name, "MINUS") == 0) { // MINUS Exp
        Operand *newOp = create_oprand(CONSTANT_OP, VAL, 0);
        Operand *op1 = translate_Exp(node1);
        Operand *op2 = create_oprand(TEMP_OP, VAL);
        generate_code(SUB_INTERCODE, op2, newOp, op1);
        ret = op2;
        return ret;
    } 
    else if (strcmp(node0->name, "NOT") == 0) { // NOT Exp
        Operand *label1 = create_oprand(LABEL_OP, VAL);
        Operand *label2 = create_oprand(LABEL_OP, VAL);
        ret = create_oprand(TEMP_OP, VAL);
        Operand *newOp = create_oprand(CONSTANT_OP, VAL, 0);
        generate_code(ASSIGN_INTERCODE, ret, newOp);

        translate_Cond(root, label1, label2);
        generate_code(LABEL_INTERCODE, label1);

        Operand *oneOp = create_oprand(CONSTANT_OP, VAL, 1);
        generate_code(ASSIGN_INTERCODE, ret, oneOp);
        generate_code(LABEL_INTERCODE, label2);

        return ret;
    } 
    else if (strcmp(node0->name, "ID") == 0) { // ID
        if (node1 == NULL) { // ID
            SymbolTableNode *IDnode = get_symboltable_node(curSnapshot->curTable, node0->value);
            if (IDnode->type->kind == ARRAY || IDnode->type->kind == STRUCTURE) {
                if (IDnode->isAddr == ADDRESS) {
                    ret = create_oprand(VARIABLE_OP, ADDRESS, node0->value);
                } else {
                    ret = create_oprand(VARIABLE_OP, VAL, node0->value);
                }
                ret->var_no = IDnode->var_no;
                ret->depth = 0;
                ret->varName = node0->value;
                
                return ret;
            } else {
                ret = create_oprand(VARIABLE_OP, VAL, node0->value);
                ret->var_no = IDnode->var_no;
                ret->depth = 0;
                ret->varName = node0->value;
                
                return ret;
            }
        } else {
            if (strcmp(node0->value, "write")==0 && strcmp(node2->name, "Args")==0) {
                astNode *expNode = get_child(node2, 0);
                Operand *tmpOp = NULL;

                if (strcmp(expNode->name, "Exp") == 0) {
                    tmpOp = translate_Exp(expNode);
                }
                if (tmpOp != NULL) {
                    generate_code(WRITE_INTERCODE, tmpOp);
                }

                Operand *constOp = create_oprand(CONSTANT_OP, VAL, 0);
                ret = create_oprand(TEMP_OP, VAL);
                generate_code(ASSIGN_INTERCODE, ret, constOp);
                return ret;
            }
            ret = create_oprand(TEMP_OP, VAL);
            
            if (strcmp(node0->value, "read") == 0) {
                
                generate_code(READ_INTERCODE, ret);
                return ret;
            }
            Operand *funcName = create_oprand(FUNCTION_OP, VAL, node0->value);
            if (strcmp(node2->name, "RP") == 0) {
                generate_code(CALL_INTERCODE, ret, funcName);
                return ret;
            } 
            else if (strcmp(node2->name, "Args") == 0) {
                FunctionTable *IDnode = get_functiontable_node(functionTable, node0->value);
                translate_Args(node2, IDnode->type->u.function.params);
                generate_code(CALL_INTERCODE, ret, funcName);
                return ret;
            }
            return ret;
        }
    } 
    else if (strcmp(node0->name, "INT") == 0) { // INT
        ret = create_oprand(CONSTANT_OP, VAL, str2int(node0->value));
        return ret;
    } 
    else if (strcmp(node0->name, "FLOAT") == 0) { // 不支持FLOAT
        ret = create_oprand(CONSTANT_OP, VAL, 0);
        return ret;
    }
    return ret;
}
/* Done */
void translate_Args(astNode *root, FieldList *field){
    /*
    Args -> Exp COMMA Args
    | Exp;
     */
    print_procedure(root, DEBUGMODE);
    if (root == NULL || field == NULL) {
        return;
    }

    astNode *Exp = get_child(root, 0);
    astNode *Args = get_child(root, 2);

    Operand *ExpOp = translate_Exp(Exp);
    Operand *tmpExpOp = copy_operand(ExpOp);
    if (field->type->kind == STRUCTURE || field->type->kind == ARRAY)
    {
        int flag = 0;
        if (field->type->kind == ARRAY)
        {
            char *name = tmpExpOp->varName;
            SymbolTableNode *arrayNode = get_symboltable_node(curSnapshot->curTable, name);
            int count = 0;
            Type *tmpType = arrayNode->type;
            while (tmpType->kind == ARRAY) {
                count++;
                tmpType = tmpType->u.array.elem;
            }
            if (tmpExpOp->depth < count) {
                flag = 1;
            }
            if (tmpExpOp->depth == 0) {
                flag = 0;
            }
        }
        if (flag == 1)
        {
            tmpExpOp->type = VAL;
        }
        else if (tmpExpOp->type == ADDRESS)
        {
            tmpExpOp->type = VAL;
        }
        else
        {
            tmpExpOp->type = ADDRESS;
        }
    }
    if (Args != NULL) {
        translate_Args(Args, field->nextFieldList);
    }
    generate_code(ARG_INTERCODE, tmpExpOp);
}
/* Done */
void translate_ParamDec(astNode *root){
    /*
    ParamDec -> Specifier VarDec
     */
    print_procedure(root, DEBUGMODE);
    astNode *VarDec = get_child(root, 1);
    if (VarDec != NULL) {
        translate_VarDec(VarDec);
    }
}
/* Done */
void translate_Cond(astNode *root, Operand *labelTrue, Operand *labelFalse){
    
    print_procedure(root, DEBUGMODE);
    Operand *newOp = create_oprand(CONSTANT_OP, VAL, 0);
    astNode *node0 = get_child(root, 0);
    astNode *node1 = get_child(root, 1);
    astNode *node2 = get_child(root, 2);
    if (node0 != NULL) {
        if (strcmp(node0->name, "Exp") == 0) {
            if (strcmp(node1->name, "ASSIGNOP") == 0) {
                Operand *op1 = translate_Exp(node0);
                Operand *op2 = translate_Exp(node2);
                generate_code(ASSIGN_INTERCODE, op1, op2);

                if (labelTrue != NULL && labelFalse != NULL) {
                    if (op1 != NULL) {
                        generate_code(IFGOTO_INTERCODE, op1, "!=", newOp, labelTrue);
                    }
                    generate_code(GOTO_INTERCODE, labelFalse);
                }
                else if (labelTrue != NULL) {
                    if (op1 != NULL) {
                        generate_code(IFGOTO_INTERCODE, op1, "!=", newOp, labelTrue);
                    }
                }
                else if (labelFalse != NULL) {
                    if (op1 != NULL) {
                        generate_code(IFGOTO_INTERCODE, op1, "==", newOp, labelFalse);
                    }
                }
            } 
            else if (strcmp(node1->name, "AND") == 0) {
                if (labelFalse != NULL) {
                    translate_Cond(node0, NULL, labelFalse);
                    translate_Cond(node2, labelTrue, labelFalse);
                } else {
                    Operand *label = create_oprand(LABEL_OP, VAL);
                    translate_Cond(node0, NULL, label);
                    translate_Cond(node2, labelTrue, labelFalse);
                    generate_code(LABEL_INTERCODE, label);
                }
            }
            else if (strcmp(node1->name, "OR") == 0) {
                if (labelTrue != NULL) {
                    translate_Cond(node0, labelTrue, NULL);
                    translate_Cond(node2, labelTrue, labelFalse);
                } else {
                    Operand *label = create_oprand(LABEL_OP, VAL);
                    translate_Cond(node0, label, NULL);
                    translate_Cond(node2, labelTrue, labelFalse);
                    generate_code(LABEL_INTERCODE, label);
                }
            }
            else if (strcmp(node1->name, "RELOP") == 0) {
                Operand *op1 = translate_Exp(node0);
                Operand *op2 = translate_Exp(node2);
                if (labelTrue != NULL && labelFalse != NULL) {
                    if (op1 != NULL) {
                        generate_code(IFGOTO_INTERCODE, op1, node1->value, op2, labelTrue);
                    }
                    generate_code(GOTO_INTERCODE, labelFalse);
                }
                else if (labelTrue != NULL) {
                    if (op1 != NULL) {
                        generate_code(IFGOTO_INTERCODE, op1, node1->value, op2, labelTrue);
                    }
                }
                else if (labelFalse != NULL) {
                    if (op1 != NULL) {
                        generate_code(IFGOTO_INTERCODE, op1, neg_relop(node1->value), op2, labelFalse);
                    }
                }
            }
            else if (strcmp(node1->name, "PLUS") == 0 || strcmp(node1->name, "MINUS") == 0 || strcmp(node1->name, "STAR") == 0 || strcmp(node1->name, "DIV") == 0) {
                Operand *op1 = translate_Exp(node0);
                Operand *op2 = translate_Exp(node2);
                int kind;
                if (strcmp(node1->name, "PLUS") == 0) {
                    kind = ADD_INTERCODE;
                } else if (strcmp(node1->name, "MINUS") == 0) {
                    kind = SUB_INTERCODE;
                } else if (strcmp(node1->name, "STAR") == 0) {
                    kind = MUL_INTERCODE;
                } else if (strcmp(node1->name, "DIV") == 0) {
                    kind = DIV_INTERCODE;
                }
                Operand *result = create_oprand(TEMP_OP, VAL);
                if (op1 != NULL && op2 != NULL) {
                    generate_code(kind, result, op1, op2);
                }
                if (labelTrue != NULL && labelFalse != NULL) {
                    generate_code(IFGOTO_INTERCODE, result, "!=", newOp, labelTrue);
                    generate_code(GOTO_INTERCODE, labelFalse);
                }
                else if (labelTrue != NULL) {
                    generate_code(IFGOTO_INTERCODE, result, "!=", newOp, labelTrue);
                }
                else if (labelFalse != NULL) {
                    generate_code(IFGOTO_INTERCODE, result, "==", newOp, labelFalse);
                }
            }
            else if (strcmp(node1->name, "LB")) {
                Operand *op = translate_Exp(root);
                if (labelTrue != NULL && labelFalse != NULL) {
                    generate_code(IFGOTO_INTERCODE, op, "!=", newOp, labelTrue);
                    generate_code(GOTO_INTERCODE, labelFalse);
                }
                else if (labelTrue != NULL) {
                    generate_code(IFGOTO_INTERCODE, op, "!=", newOp, labelTrue);
                }
                else if (labelFalse != NULL) {
                    generate_code(IFGOTO_INTERCODE, op, "==", newOp, labelFalse);
                }
            }
            else if (strcmp(node1->name, "DOT") == 0) {
                Operand *op = translate_Exp(root);
                if (labelTrue != NULL && labelFalse != NULL) {
                    generate_code(IFGOTO_INTERCODE, op, "!=", newOp, labelTrue);
                    generate_code(GOTO_INTERCODE, labelFalse);
                }
                else if (labelTrue != NULL) {
                    generate_code(IFGOTO_INTERCODE, op, "!=", newOp, labelTrue);
                }
                else if (labelFalse != NULL) {
                    generate_code(IFGOTO_INTERCODE, op, "==", newOp, labelFalse);
                }
            }
        }
        else if (strcmp(node0->name, "LP") == 0) {
            translate_Cond(node1, labelTrue, labelFalse);
        }
        else if (strcmp(node0->name, "MINUS") == 0) {
            Operand *op = translate_Exp(root);
            if (labelTrue != NULL && labelFalse != NULL) {
                generate_code(IFGOTO_INTERCODE, op, "!=", newOp, labelTrue);
                generate_code(GOTO_INTERCODE, labelFalse);
            }
            else if (labelTrue != NULL) {
                generate_code(IFGOTO_INTERCODE, op, "!=", newOp, labelTrue);
            }
            else if (labelFalse != NULL) {
                generate_code(IFGOTO_INTERCODE, op, "==", newOp, labelFalse);
            }
        }
        else if (strcmp(node0->name, "NOT") == 0) {
            translate_Cond(node1, labelFalse, labelTrue);
        }
        else if (strcmp(node0->name, "ID") == 0) {
            Operand *op = translate_Exp(root);
            if (labelTrue != NULL && labelFalse != NULL) {
                generate_code(IFGOTO_INTERCODE, op, "!=", newOp, labelTrue);
                generate_code(GOTO_INTERCODE, labelFalse);
            }
            else if (labelTrue != NULL) {
                generate_code(IFGOTO_INTERCODE, op, "!=", newOp, labelTrue);
            }
            else if (labelFalse != NULL) {
                generate_code(IFGOTO_INTERCODE, op, "==", newOp, labelFalse);
            }
        } 
        else if (strcmp(node0->name, "INT") == 0) {
            if (labelTrue != NULL && (str2int(node0->value) != 0)) {
                generate_code(GOTO_INTERCODE, labelTrue);
            }
            if (labelFalse != NULL && (str2int(node0->value) == 0)) {
                generate_code(GOTO_INTERCODE, labelFalse);
            }
        }  
    }
}


/**************************************`************************************** 
* 数据结构InterCode提供的方法
****************************************************************************/
InterCodeList *init_intercodelist(){
    InterCodeList *headCode = (InterCodeList *)malloc(sizeof(InterCodeList));
    headCode->next = NULL;
    headCode->prev = NULL;
    return headCode;
}

void insert_intercodelist(InterCodeList *curCode){
    curCode->next = headCode;
    curCode->prev = tailCode;
    tailCode->next = curCode;
    headCode->prev = curCode;
    tailCode = curCode;
}

/* 融合了create和insert，直接根据类型并提供操作数，创建并插入一个InterCode */
void generate_code(int kind, ...){
    va_list oprands;
    va_start(oprands, kind);
    /* create code */
    InterCodeList *newCode = (InterCodeList*)(malloc(sizeof(InterCodeList)));
    newCode->code.kind = kind;
    newCode->next = NULL;
    switch (kind) {
        /* Single OP */
        case LABEL_INTERCODE:
        case FUNC_INTERCODE:
        case GOTO_INTERCODE:
        case RETURN_INTERCODE:
        case ARG_INTERCODE:
        case PARAM_INTERCODE:
        case READ_INTERCODE:
        case WRITE_INTERCODE: 
        {
            newCode->code.u.singleOP.op = va_arg(oprands, Operand*);
            break;
        }
        /* Double OP */
        case ASSIGN_INTERCODE:
        case GET_ADDR_INTERCODE:
        case GET_CONTENT_INTERCODE:
        case TO_ADDR_INTERCODE:
        case CALL_INTERCODE:
        case DEC_INTERCODE:
        {
            newCode->code.u.doubleOP.left = va_arg(oprands, Operand*);
            newCode->code.u.doubleOP.right = va_arg(oprands, Operand*);
            break;
        }
        /* Triple OP */
        case ADD_INTERCODE:
        case SUB_INTERCODE:
        case MUL_INTERCODE:
        case DIV_INTERCODE:
        {
            newCode->code.u.tripleOP.result = va_arg(oprands, Operand*);
            newCode->code.u.tripleOP.op1 = va_arg(oprands, Operand*);
            newCode->code.u.tripleOP.op2 = va_arg(oprands, Operand*);
            break;
        }
        /* Jump OP */
        case IFGOTO_INTERCODE:
        {
            newCode->code.u.ifgotoOP.op1 = va_arg(oprands, Operand*);
            newCode->code.u.ifgotoOP.relop = va_arg(oprands, char *);
            newCode->code.u.ifgotoOP.op2 = va_arg(oprands, Operand*);
            newCode->code.u.ifgotoOP.label = va_arg(oprands, Operand*);
            break;
        }
    }
    /* insert code */
    insert_intercodelist(newCode);
    va_end(oprands);
}

/* 创建操作数 */
Operand* create_oprand(int kind, int type, ...)
{
    va_list args;
    va_start(args, type);
    Operand* newOperand = (Operand*)(malloc(sizeof(Operand)));
    newOperand->kind = kind;
    newOperand->type = type;
    switch (kind) {
        case VARIABLE_OP: {//变量
            newOperand->var_no = varNo;
            varNo++;
            newOperand->varName = va_arg(args, char *);
        } break;

        case CONSTANT_OP: {//常量
            newOperand->value = va_arg(args, int);
        } break;

        case TEMP_OP: {//临时变量
            newOperand->var_no = tempNo;
            tempNo++;
        } break;

        case FUNCTION_OP: {//函数
            newOperand->funcName = va_arg(args, char *);
        } break;

        case LABEL_OP: {//标号
            newOperand->var_no = labelNo;
            labelNo++;
        } break;
    }

    va_end(args);
    return newOperand;
}

/**************************************************************************** 
* 根据当前的快照，以及全局的符号表和作用域，进入或退出作用域，以恢复当前情况的下的快照
****************************************************************************/
void enter_scope(snapshot *curSnapshot) {
    // TODO：进入内层作用域（对应push）
}

void exit_scope(snapshot *curSnapshot) {
    // TODO：退出内层作用域（对应pop）
}

void enter_innermost_scope(snapshot *curSnapshot) {
    // TODO：进入最内层作用域（用来初始化到全局的作用域）
}

/**************************************************************************** 
* 辅助函数
****************************************************************************/
void printe_oprand(Operand *op, FILE *file) {
    switch (op->kind) {
        case VARIABLE_OP: {//变量
            if (op->type == ADDRESS) {
                fprintf(file, "&");
            }
            fprintf(file, "v%d", op->var_no);
        } break;

        case CONSTANT_OP: {//常量
            fprintf(file, "#%d", op->value);
        } break;

        case TEMP_OP: {//临时变量
            if (op->type == ADDRESS) {
                fprintf(file, "*");
            }
            fprintf(file, "t%d", op->var_no);
        } break;

        case FUNCTION_OP: {//函数
            fprintf(file, "%s", op->funcName);
        } break;

        case LABEL_OP: {//标号
            fprintf(file, "%d", op->var_no);
        } break;
    }
}

/* 打印中间代码 */
void print_intercode(InterCodeList *headCode, FILE *file) {
    InterCodeList *curCode = headCode->next;
    while (curCode != headCode) {
        switch (curCode->code.kind) {
            case LABEL_INTERCODE: { //定义标号x LABEL x :
                fprintf(file, "LABEL label");
                printe_oprand(curCode->code.u.singleOP.op, file);
                fprintf(file, " : \n");
            } break;

            case FUNC_INTERCODE: { //定义函数f FUNCTION f :
                fprintf(file, "FUNCTION ");
                printe_oprand(curCode->code.u.singleOP.op, file);
                fprintf(file, " : \n");
            } break;

            case ASSIGN_INTERCODE: { //赋值操作 x := y
                printe_oprand(curCode->code.u.doubleOP.left, file);
                fprintf(file, " := ");
                printe_oprand(curCode->code.u.doubleOP.right, file);
                fprintf(file, "\n");
            } break;

            case ADD_INTERCODE: { //加法操作 x := y + z
                printe_oprand(curCode->code.u.tripleOP.result, file);
                fprintf(file, " := ");
                printe_oprand(curCode->code.u.tripleOP.op1, file);
                fprintf(file, " + ");
                printe_oprand(curCode->code.u.tripleOP.op2, file);
                fprintf(file, "\n");
            } break;

            case SUB_INTERCODE: { //减法操作 x := y - z
                printe_oprand(curCode->code.u.tripleOP.result, file);
                fprintf(file, " := ");
                printe_oprand(curCode->code.u.tripleOP.op1, file);
                fprintf(file, " - ");
                printe_oprand(curCode->code.u.tripleOP.op2, file);
                fprintf(file, "\n");
            } break;

            case MUL_INTERCODE: { //乘法操作 x := y * z
                printe_oprand(curCode->code.u.tripleOP.result, file);
                fprintf(file, " := ");
                printe_oprand(curCode->code.u.tripleOP.op1, file);
                fprintf(file, " * ");
                printe_oprand(curCode->code.u.tripleOP.op2, file);
                fprintf(file, "\n");
            } break;

            case DIV_INTERCODE: { //除法操作 x := y / z
                printe_oprand(curCode->code.u.tripleOP.result, file);
                fprintf(file, " := ");
                printe_oprand(curCode->code.u.tripleOP.op1, file);
                fprintf(file, " / ");
                printe_oprand(curCode->code.u.tripleOP.op2, file);
                fprintf(file, "\n");
            } break;

            case GET_ADDR_INTERCODE: { //取y的地址赋给x x := &y
                printe_oprand(curCode->code.u.doubleOP.left, file);
                fprintf(file, " := &");
                printe_oprand(curCode->code.u.doubleOP.right, file);
                fprintf(file, "\n");
            } break;

            case GET_CONTENT_INTERCODE: { //取以y值为地址的内存单元的内容赋给x x := *y
                printe_oprand(curCode->code.u.doubleOP.left, file);
                fprintf(file, " := *");
                printe_oprand(curCode->code.u.doubleOP.right, file);
                fprintf(file, "\n");
            } break;

            case TO_ADDR_INTERCODE: { //取y值赋给以x值为地址的内存单元 *x := y
                fprintf(file, "*");
                printe_oprand(curCode->code.u.doubleOP.left, file);
                fprintf(file, " := ");
                printe_oprand(curCode->code.u.doubleOP.right, file);
                fprintf(file, "\n");
            } break;

            case GOTO_INTERCODE: { //无条件跳转至标号x GOTO x
                fprintf(file, "GOTO label");
                printe_oprand(curCode->code.u.singleOP.op, file);
                fprintf(file, "\n");
            } break;

            case IFGOTO_INTERCODE: { //如果x与y满足[relop]关系则跳转至标号z IF x [relop] y GOTO z
                fprintf(file, "IF ");
                printe_oprand(curCode->code.u.ifgotoOP.op1, file);
                fprintf(file, " %s ", curCode->code.u.ifgotoOP.relop);
                printe_oprand(curCode->code.u.ifgotoOP.op2, file);
                fprintf(file, " GOTO label");
                printe_oprand(curCode->code.u.ifgotoOP.label, file);
                fprintf(file, "\n");
            } break;

            case RETURN_INTERCODE: { //退出当前函数并返回x值 RETURN x
                fprintf(file, "RETURN ");
                printe_oprand(curCode->code.u.singleOP.op, file);
                fprintf(file, "\n");
            } break;

            case DEC_INTERCODE: { //内存空间申请，大小为4的倍数 DEC x [size]
                fprintf(file, "DEC ");
                printe_oprand(curCode->code.u.doubleOP.left, file);
                fprintf(file, " %d", curCode->code.u.doubleOP.right->value);
                fprintf(file, "\n");
            } break;

            case ARG_INTERCODE: { //传实参x ARG x
                fprintf(file, "ARG ");
                printe_oprand(curCode->code.u.singleOP.op, file);
                fprintf(file, "\n");
            } break;

            case CALL_INTERCODE: { //调用函数，并将其返回值赋给x x := CALL f
                printe_oprand(curCode->code.u.doubleOP.left, file);
                fprintf(file, " := CALL ");
                printe_oprand(curCode->code.u.doubleOP.right, file);
                fprintf(file, "\n");
            } break;

            case PARAM_INTERCODE: { //函数参数声明 PARAM x
                fprintf(file, "PARAM ");
                printe_oprand(curCode->code.u.singleOP.op, file);
                fprintf(file, "\n");
            } break;

            case READ_INTERCODE: { //从控制台读取x的值 READ x
                fprintf(file, "READ ");
                printe_oprand(curCode->code.u.singleOP.op, file);
                fprintf(file, "\n");
            } break;

            case WRITE_INTERCODE: { //向控制台打印x的值 WRITE x
                fprintf(file, "WRITE ");
                printe_oprand(curCode->code.u.singleOP.op, file);
                fprintf(file, "\n");
            } break;
        }
        curCode = curCode->next;
    }
}

/* 复制操作数 */
Operand *copy_operand(Operand *operand) {
    Operand *newOperand = (Operand*)malloc(sizeof(Operand));
    newOperand->kind = operand->kind;
    newOperand->type = operand->type;
    newOperand->varName = operand->varName;
    newOperand->var_no = operand->var_no;
    newOperand->funcName = operand->funcName;
    newOperand->value = operand->value;
    newOperand->depth = operand->depth;
    return newOperand;
}

/* 判断符号取反 */
char *neg_relop(char *relop) {
    char *ret = NULL;
    if (strcmp(relop, ">") == 0) {
        ret = "<=";
    }
    else if (strcmp(relop, "<") == 0) {
        ret = ">=";
    }
    else if (strcmp(relop, ">=") == 0) {
        ret = "<";
    }
    else if (strcmp(relop, "<=") == 0) {
        ret = ">";
    }
    else if (strcmp(relop, "==") == 0) {
        ret = "!=";
    }
    else if (strcmp(relop, "!=") == 0) {
        ret = "==";
    }

    return ret;
}

/* 获取类型大小 */
int get_size(Type *type) {
    if (type->kind == BASIC) {
        return 4;
    }
    else if (type->kind == ARRAY) {
        int arraySize = 1;
        Type *tmpType = type;
        while (tmpType != NULL) {
            if (tmpType->kind != ARRAY)
                break;
            arraySize *= tmpType->u.array.size;
            tmpType = tmpType->u.array.elem;
        }
        arraySize *= get_size(tmpType);
        return arraySize;
    }
    else if (type->kind == STRUCTURE) {
        int structSize = 0;
        FieldList *tmpStruct = type->u.structure.structures;
        while (tmpStruct != NULL)
        {
            structSize += get_size(tmpStruct->type);
            tmpStruct = tmpStruct->nextFieldList;
        }
        return structSize;
    }
    assert(0);
}

void insert_fieldlist(FieldList *field){
    /*
    struct FieldList_ {
    char *name;
    Type *type; 
    FieldList *nextFieldList;
    };
     */
    int curOffset = 0;
    FieldList *curField = field;
    field_print(curField, DEBUGMODE);
    while(curField != NULL) {
        SymbolTableNode *curNode = create_symboltable_node(curField->type, curField->name, 0);
        curNode->offset = curOffset;//对于结构体的域名，我们只关心其偏移
        insert_symboltable_node(curSnapshot->curTable, curSnapshot->curStack, curNode);
        curOffset += get_size(curField->type);
        curField = curField->nextFieldList;
    }
}

/* 最开始将所有的结构体的域名，计算其偏差并插入到符号表 */
void insert_struct(StructureTable *structTable){
    StructureTable *curStruct = structTable;
    while(curStruct != NULL) {
        if (curStruct->structNode != NULL) {
            FieldList *curFieldList = curStruct->structNode->type->u.structure.structures;
            insert_fieldlist(curFieldList);
        }
        curStruct = curStruct->nextStruct;
    }
}