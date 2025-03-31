#ifndef __SEMANTIC_H__
#define __SEMANTIC_H__

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

#endif