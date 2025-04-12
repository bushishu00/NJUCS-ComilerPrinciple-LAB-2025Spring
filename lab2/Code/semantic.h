#ifndef __SEMANTIC_H__
#define __SEMANTIC_H__

#include "symboltable.h"
#include "astnode.h"
typedef enum SemanticError
{
    Undefined_Variable,
    Undefined_Function,
    Redefined_Variable_Name,
    Redefined_Function,
    AssignOP_Type_Dismatch,
    Leftside_Rvalue_Error,
    Operand_Type_Dismatch,
    Return_Type_Dismatch,
    Func_Call_Parameter_Dismatch,
    Operate_Others_As_Array,
    Operate_Basic_As_Func,
    Array_Float_Index,
    Operate_Others_As_Struct,
    Undefined_Field,
    Redefined_Field,
    Redefined_Field_Name,
    Undefined_Struct,
    Undefined_Function_But_Declaration,
    Conflict_Decordef_Funcion
} SemanticError;
/* Program 语义分析起点 */
void Program(astNode *root);
/* ExtDefList ExtDefList检查 */
void ExtDefList(astNode *root);
/* ExtDef ExtDef检查 */
void ExtDef(astNode *root);
/* ExtDecList ExtDecList检查 */
void ExtDecList(astNode *root, Type *type);
/* Specifier Specifier检查 */
Type *Specifier(astNode *root);
Type *StructSpecifier(astNode *root);
FieldList *DefList_in_struct(astNode *root);
FieldList *Def_in_struct(astNode *root);
FieldList *DecList_in_struct(astNode *root, Type *type);
FieldList *Dec_in_struct(astNode *root, Type *type);
/* VarDec VarDec检查 */
FieldList *VarDec(astNode *root, Type *type);
/* FunDec FunDec检查 */
int FunDec(astNode *root, bool isDefined, Type *type);
/* VarList VarList检查 */
FieldList *VarList(astNode *root);
/* ParamDec ParamDec检查 */
FieldList *ParamDec(astNode *root);
/* CompSt CompSt检查 */
void CompSt(astNode *root, Type *type);
/* StmtList StmtList检查 */
void StmtList(astNode *root, Type *type);
/* Stmt Stmt检查 */
void Stmt(astNode *root, Type *type);
/* DefList DefList检查 */
void DefList(astNode *root);
/* Def Def检查 */
void Def(astNode *root);
/* DecList Defcist检查 */
void DecList(astNode *root, Type *type);
/* Dec Dec检查 */
void Dec(astNode *root, Type *type);
/* Exp Exp检查 */
Type *Exp(astNode *root);
/* Args Args检查 */
FieldList *Args(astNode *root);

void check_undef_func(void);
void semantic_error(enum SemanticError errortype, int line, char *message);
void scope_begin(void);
void scope_end(void);
void insert_valid_fieldlist(FieldList *field);
SymbolTableNode *get_scope_first_node(void);
int get_params_num(FieldList *params);
bool check_condition_type(Type *type);
bool check_var_redefined(char *name);
bool check_struct_redefined(char *name);
bool check_type_equal(Type *a, Type *b);
bool array_strong_equal(Type *a, Type *b);
bool check_fieldlist_equal(FieldList *a, FieldList* b);
int check_var_struct_redefined(char *name, int curType);
bool check_fieldlist_redefined(FieldList *field, char *name);

#endif