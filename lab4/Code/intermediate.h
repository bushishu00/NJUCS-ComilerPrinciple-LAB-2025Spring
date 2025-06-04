#ifndef __INTERMEDIATE_H__
#define __INTERMEDIATE_H__
#include "symboltable.h"
#include "astnode.h"

/**************************************************************************** 
* 数据结构定义
****************************************************************************/
typedef struct Operand Operand;
typedef struct InterCode InterCode;
typedef struct InterCodeList InterCodeList;
typedef struct snapshot snapshot;

struct Operand
{
    enum
    {
        VARIABLE_OP, //变量
        CONSTANT_OP, //常量
        TEMP_OP,     //临时变量
        FUNCTION_OP, //函数
        LABEL_OP     //标号
    } kind;
    enum
    {
        VAL,
        ADDRESS
    } type;
    int var_no;     //标号数
    int value;      //值
    char *varName;  //变量名
    char *funcName; //函数名
    int depth;
};
struct InterCode
{
    enum
    {
        LABEL_INTERCODE,       //定义标号x LABEL x :
        FUNC_INTERCODE,        //定义函数f FUNCTION f :
        ASSIGN_INTERCODE,      //赋值操作 x := y
        ADD_INTERCODE,         //加法操作 x := y + z
        SUB_INTERCODE,         //减法操作 x := y - z
        MUL_INTERCODE,         //乘法操作 x := y * z
        DIV_INTERCODE,         //除法操作 x := y / z
        GET_ADDR_INTERCODE,    //取y的地址赋给x x := &y
        GET_CONTENT_INTERCODE, //取以y值为地址的内存单元的内容赋给x x := *y
        TO_ADDR_INTERCODE,     //取y值赋给以x值为地址的内存单元 *x := y
        GOTO_INTERCODE,        //无条件跳转至标号x GOTO x
        IFGOTO_INTERCODE,      //如果x与y满足[relop]关系则跳转至标号z IF x [relop] y GOTO z
        RETURN_INTERCODE,      //退出当前函数并返回x值 RETURN x
        DEC_INTERCODE,         //内存空间申请，大小为4的倍数 DEC x [size]
        ARG_INTERCODE,         //传实参x ARG x
        CALL_INTERCODE,        //调用函数，并将其返回值赋给x x := CALL f
        PARAM_INTERCODE,       //函数参数声明 PARAM x
        READ_INTERCODE,        //从控制台读取x的值 READ x
        WRITE_INTERCODE        //向控制台打印x的值 WRITE x
    } kind;
    union
    {
        struct
        {
            Operand *op;
        } singleOP; // LABEL FUNC GOTO RETURN ARG PARAM READ WRITE
        struct
        {
            Operand *left, *right;
        } doubleOP; // ASSIGN GET_ADDR GET_CONTENT TO_ADDR CALL DEC
        struct
        {
            Operand *result, *op1, *op2;
        } tripleOP; // ADD SUB MUL DIV
        struct
        {
            Operand *op1, *op2, *label;
            char *relop;
        } ifgotoOP; // IFGOTO
    } u;
};
/* 双向循环链表 */
struct InterCodeList
{
    InterCode code;
    InterCodeList *prev, *next;
}; 

struct snapshot {
    HashTable *curTable;
    ScopeStack *curStack;
};
/**************************************************************************** 
* 翻译函数
****************************************************************************/
/* translate_Program Program翻译 */
void translate_Program(astNode *root, FILE *file);
/* translate_ExtDefList  ExtDefList翻译 */
void translate_ExtDefList(astNode *root);
/* translate_ExtDef ExtDef翻译 */
void translate_ExtDef(astNode *root);
/* translate_ExtDecList ExtDecList翻译 */
void translate_ExtDecList(astNode *root);
/* translate_VarDec VarDec翻译 */
Operand *translate_VarDec(astNode *root);
/* translate_FunDec FunDec翻译 */
void translate_FunDec(astNode *root);
/* translate_VarList VarList翻译 */
void translate_VarList(astNode *root);
/* translate_CompSt CompSt翻译 */
void translate_CompSt(astNode *root);
/* translate_StmtList StmtList翻译 */
void translate_StmtList(astNode *root);
/* translate_Stmt Stmt翻译 */
void translate_Stmt(astNode *root);
/* translate_DefList DefList翻译 */
void translate_DefList(astNode *root);
/* translate_DefList Def翻译 */
void translate_Def(astNode *root);
/* translate_DecList DecList翻译 */
void translate_DecList(astNode *root);
/* translate_Dec Dec翻译 */
void translate_Dec(astNode *root);
/* translate_Exp Exp翻译 */
Operand *translate_Exp(astNode *root);
/* translate_Args Args翻译 */
void translate_Args(astNode *root, FieldList *field);
/* translate_ParamDec ParamDec翻译 */
void translate_ParamDec(astNode *root);
/* translate_Cond Cond翻译 */
void translate_Cond(astNode *root, Operand *lableTure, Operand *lableFalse);



/**************************************************************************** 
* 数据结构与功能函数
****************************************************************************/
InterCodeList *init_intercodelist(void);

void insert_intercodelist(InterCodeList *curCode);

void generate_code(int kind, ...);

Operand* create_oprand(int kind, int type, ...);

void printe_oprand(Operand *op, FILE *file);

void print_intercode(InterCodeList *headCode, FILE *file);

Operand *copy_operand(Operand *operand);

char *neg_relop(char *relop);

int get_size(Type *type);


void enter_scope(snapshot *curSnapshot);

void exit_scope(snapshot *curSnapshot);

void enter_innermost_scope(snapshot *curSnapshot);

void insert_struct(StructureTable *structTable);

#endif