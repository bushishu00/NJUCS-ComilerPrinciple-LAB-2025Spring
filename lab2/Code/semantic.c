#include "semantic.h"


void printSemanticError(enum SemanticError errortype, int line, char *message)
{
    printf("Error type %d at Line %d: ", errortype + 1, line);
    switch (errortype)
    {
    case Undefined_Variable:
    {
        printf("Variable \"%s\" is used but haven't been defined.\n", message);
    }
    break;
    case Undefined_Function:
    {
        printf("Function \"%s\" is called but haven't been defined", message);
    }
    break;
    case Redefined_Variable_Name:
    {
        printf("Variable \"%s\" is defined more than once", message);
    }
    break;
    case Redefined_Function:
    {
        printf("Function \"%s\" is defined more than once", message);
    }
    break;
    case AssignOP_Type_Dismatch:
    {
        printf("The expression types on both sides of the assignment do not match.\n");
    }
    break;
    case Leftside_Rvalue_Error:
    {
        printf("Only a rvalue-expression appears to the left of an assignment.\n");
    }
    break;
    case Operand_Type_Dismatch:
    {
        printf("Operand types are mismatched or operand type and operator are mismatched.\n");
    }
    break;
    case Return_Type_Dismatch:
    {
        printf("Return type is mismatched.\n");
    }
    break;
    case Func_Call_Parameter_Dismatch:
    {
        printf("Parameters are mismatched when calling the function.\n");
    }
    break;
    case Operate_Others_As_Array:
    {
        printf("Operate other types of operand as array.\n");
    }
    break;
    case Operate_Basic_As_Func:
    {
        printf("Operate other types of operand as function.\n");
    }
    break;
    case Array_Float_Index:
    {
        printf("Use float as an array index.\n");
    }
    break;
    case Operate_Others_As_Struct:
    {
        printf("Operate other types of operand as structure.\n");
    }
    break;
    case Undefined_Field:
    {
        printf("Visit an unexist structure field \"%s\".\n", message);
    }

    break;
    case Redefined_Field:
    {
        printf("Field \"%s\" is defined more than once or initialized.\n", message);
    }
    break;
    case Redefined_Field_Name:
    {
        printf("The name of the structure \"%s\" duplicates the name of a defined structure or variable.\n", message);
    }
    break;
    case Undefined_Struct:
    {
        printf("Structure \"%s\" is used but haven't been defined.\n", message);
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