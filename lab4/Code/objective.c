#include "objective.h"
#define HEAD ".data\n_prompt: .asciiz \"Enter an integer:\"\n_ret: .asciiz \"\\n\"\n.globl main\n.text\n" \
                "read:\n\tli $v0, 4\n\tla $a0, _prompt\n\tsyscall\n\tli $v0, 5\n\tsyscall\n\tjr $ra\n\n"     \
                "write:\n\tli $v0, 1\n\tsyscall\n\tli $v0, 4\n\tla $a0, _ret\n\tsyscall\n\tmove $v0, $0\n\tjr $ra\n\n"

/* Refer to MIPS spec, define 32 regs */
Register regs[32];
char *regName[32] = {
    "$zero", "$at", "$v0", "$v1", "$a0", "$a1", "$a2", "$a3", 
    "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7", 
    "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7", 
    "$t8", "$t9", "$k0", "$k1", "$gp", "$sp", "$fp", "$ra"};

/* Other global Var */
int localOffset = 0;
VarOffset *headVarOffset = NULL;

extern InterCodeList *headCode;

/* Only generate objcode in a simple way */
void generate_object_code(FILE *file) {
    init_all_regs();
    fprintf(file, HEAD);
    InterCodeList *curCode = headCode->next;
    while (curCode != headCode) {
        switch(curCode->code.kind) {
        case LABEL_INTERCODE: {
            objcode_label(curCode, file);
            break;
        } 
        case FUNC_INTERCODE: {
            objcode_func(curCode, file);
            break;
        } 
        case ASSIGN_INTERCODE: {
            objcode_assign(curCode, file);
            break;
        } 
        case ADD_INTERCODE: {
            objcode_add(curCode, file);
            break;
        } 
        case SUB_INTERCODE: {
            objcode_sub(curCode, file);
            break;
        } 
        case MUL_INTERCODE: {
            objcode_mul(curCode, file);
            break;
        } 
        case DIV_INTERCODE: {
            objcode_div(curCode, file);
            break;
        } 
        case GET_ADDR_INTERCODE: {
            objcode_get_addr(curCode, file);
            break;
        } 
        case GET_CONTENT_INTERCODE: {
            objcode_get_content(curCode, file);
            break;
        } 
        case TO_ADDR_INTERCODE: {
            objcode_to_addr(curCode, file);
            break;
        } 
        case GOTO_INTERCODE: {
            objcode_goto(curCode, file);
            break;
        } 
        case IFGOTO_INTERCODE: {
            objcode_ifgoto(curCode, file);
            break;
        } 
        case RETURN_INTERCODE: {
            objcode_return(curCode, file);
            break;
        } 
        case DEC_INTERCODE: {
            objcode_dec(curCode, file);
            break;
        } 
        case ARG_INTERCODE: {
            objcode_arg(curCode, file);
            while (curCode->code.kind != CALL_INTERCODE) {
                curCode = curCode->next;
            }
            break;
        } 
        case CALL_INTERCODE: {
            objcode_call(curCode, file);
            break;
        } 
        case PARAM_INTERCODE: {
            objcode_param(curCode, file);
            break;
        } 
        case READ_INTERCODE: {
            objcode_read(curCode, file);
            break;
        } 
        case WRITE_INTERCODE: {
            objcode_write(curCode, file);
            break;
        } 
        default: {
            assert(0);
        }
        }
        curCode = curCode->next;
    }
    return;
}

void objcode_label(InterCodeList *curCode, FILE *file){
    fprintf(file, "label%d:\n", curCode->code.u.singleOP.op->var_no);
    return;
}
void objcode_func(InterCodeList *curCode, FILE *file){
    fprintf(file, "%s:\n", curCode->code.u.singleOP.op->funcName);
    fprintf(file, "\taddi $sp, $sp, -8\n");
    fprintf(file, "\tsw $fp, 0($sp)\n");
    fprintf(file, "\tsw $ra, 4($sp)\n");
    fprintf(file, "\tmove $fp, $sp\n");
    localOffset = 0; // reset local offset for each function
    int paramNum = 0;
    InterCodeList *tmpCode = curCode->next;
    while (tmpCode->code.kind == PARAM_INTERCODE) {
        VarOffset *param = (VarOffset *)malloc(sizeof(VarOffset));
        strcpy(param->name, tmpCode->code.u.singleOP.op->varName);
        param->offset = 8 + paramNum * 4;
        paramNum++;
        /* update head */
        param->next = headVarOffset;
        headVarOffset = param;
        /* update loop */
        tmpCode = tmpCode->next;
    }
    while (tmpCode != NULL && tmpCode->code.kind != FUNC_INTERCODE) {
        switch (tmpCode->code.kind) {
        case ASSIGN_INTERCODE: {
            update_varoffset(tmpCode->code.u.doubleOP.left);
            update_varoffset(tmpCode->code.u.doubleOP.right);
            break;
        }
        case ADD_INTERCODE:
        case SUB_INTERCODE:
        case MUL_INTERCODE:
        case DIV_INTERCODE: {
            update_varoffset(tmpCode->code.u.tripleOP.op1);
            update_varoffset(tmpCode->code.u.tripleOP.op2);
            update_varoffset(tmpCode->code.u.tripleOP.result);
            break;
        }
        case DEC_INTERCODE: {
            localOffset += tmpCode->code.u.doubleOP.right->value;
            VarOffset *array = (VarOffset *)malloc(sizeof(VarOffset));
            strcpy(array->name, tmpCode->code.u.doubleOP.left->varName);
            array->offset = (-1) * localOffset;
            array->next = headVarOffset;
            headVarOffset = array;
            break;
        }
        case IFGOTO_INTERCODE: {
            update_varoffset(tmpCode->code.u.ifgotoOP.op1);
            update_varoffset(tmpCode->code.u.ifgotoOP.op2);
            break;
        }
        case CALL_INTERCODE: {
            update_varoffset(tmpCode->code.u.doubleOP.left);
            break;
        }
        case ARG_INTERCODE:
        case WRITE_INTERCODE:
        case READ_INTERCODE: {
            update_varoffset(tmpCode->code.u.singleOP.op);
            break;
        }
        default:
            break;
        }
        tmpCode = tmpCode->next;
    }
    fprintf(file, "\taddi $sp, $sp, %d\n", (-1) * localOffset);
    deactivate_temp_regs();
    return;
}

void objcode_assign(InterCodeList *curCode, FILE *file){
    Operand *leftOp = curCode->code.u.doubleOP.left;
    Operand *rightOp = curCode->code.u.doubleOP.right;
    int rightID = get_reg(rightOp, file);
    int leftID = 8;
    if (leftOp->kind == TEMP_OP && leftOp->type == ADDRESS) {
        VarOffset *leftVarOffset = get_varoffset(leftOp);
        for (int i = 8; i <= 15; i++)
            if (regs[i].used == 0)
                leftID = i;
        regs[leftID].used = 1;
        fprintf(file, "lw %s, %d($fp)\n", regs[leftID].name, leftVarOffset->offset);
        fprintf(file, "sw %s, 0(%s)\n", regs[rightID].name, regs[leftID].name);
        regs[leftID].used = 0;
        regs[rightID].used = 0;
    } else {
        leftID = get_reg(leftOp, file);
        fprintf(file, "\tmove %s, %s\n", regs[leftID].name, regs[rightID].name);
        save2mem(leftID, file);
    }
    return;
} 
void objcode_add(InterCodeList *curCode, FILE *file){
    int resID = get_reg(curCode->code.u.tripleOP.result, file);
    int op1ID = get_reg(curCode->code.u.tripleOP.op1, file);
    int op2ID = get_reg(curCode->code.u.tripleOP.op2, file);

    fprintf(file, "\tadd %s, %s, %s\n", regs[resID].name, regs[op1ID].name, regs[op2ID].name);
    save2mem(resID, file);
    return;
}
void objcode_sub(InterCodeList *curCode, FILE *file){
    int resID = get_reg(curCode->code.u.tripleOP.result, file);
    int op1ID = get_reg(curCode->code.u.tripleOP.op1, file);
    int op2ID = get_reg(curCode->code.u.tripleOP.op2, file);

    fprintf(file, "\tsub %s, %s, %s\n", regs[resID].name, regs[op1ID].name, regs[op2ID].name);
    save2mem(resID, file);
    return;
}
void objcode_mul(InterCodeList *curCode, FILE *file){
    int resID = get_reg(curCode->code.u.tripleOP.result, file);
    int op1ID = get_reg(curCode->code.u.tripleOP.op1, file);
    int op2ID = get_reg(curCode->code.u.tripleOP.op2, file);

    fprintf(file, "\tmul %s, %s, %s\n", regs[resID].name, regs[op1ID].name, regs[op2ID].name);
    save2mem(resID, file);
    return;
} 
void objcode_div(InterCodeList *curCode, FILE *file){
    int resID = get_reg(curCode->code.u.tripleOP.result, file);
    int op1ID = get_reg(curCode->code.u.tripleOP.op1, file);
    int op2ID = get_reg(curCode->code.u.tripleOP.op2, file);

    fprintf(file, "\tdiv %s, %s, %s\n", regs[resID].name, regs[op1ID].name, regs[op2ID].name);
    save2mem(resID, file);
    return;
}
void objcode_get_addr(InterCodeList *curCode, FILE *file){
    return;
}
void objcode_get_content(InterCodeList *curCode, FILE *file){
    return;
}
void objcode_to_addr(InterCodeList *curCode, FILE *file){
    return;
}
void objcode_goto(InterCodeList *curCode, FILE *file){
    fprintf(file, "\tj label%d\n", curCode->code.u.singleOP.op->var_no);
    return;
}
void objcode_ifgoto(InterCodeList *curCode, FILE *file){
    int op1ID = get_reg(curCode->code.u.ifgotoOP.op1, file);
    int op2ID = get_reg(curCode->code.u.ifgotoOP.op2, file);
    if (strcmp(curCode->code.u.ifgotoOP.relop, "==") == 0) {
        fprintf(file, "\tbeq %s, %s, label%d\n", regs[op1ID].name, regs[op2ID].name, curCode->code.u.ifgotoOP.label->var_no);
    }
    else if (strcmp(curCode->code.u.ifgotoOP.relop, "!=") == 0) {
        fprintf(file, "\tbne %s, %s, label%d\n", regs[op1ID].name, regs[op2ID].name, curCode->code.u.ifgotoOP.label->var_no);
    }
    else if (strcmp(curCode->code.u.ifgotoOP.relop, ">") == 0) {
        fprintf(file, "\tbgt %s, %s, label%d\n", regs[op1ID].name, regs[op2ID].name, curCode->code.u.ifgotoOP.label->var_no);
    }
    else if (strcmp(curCode->code.u.ifgotoOP.relop, "<") == 0) {
        fprintf(file, "\tblt %s, %s, label%d\n", regs[op1ID].name, regs[op2ID].name, curCode->code.u.ifgotoOP.label->var_no);
    }
    else if (strcmp(curCode->code.u.ifgotoOP.relop, ">=") == 0) {
        fprintf(file, "\tbge %s, %s, label%d\n", regs[op1ID].name, regs[op2ID].name, curCode->code.u.ifgotoOP.label->var_no);
    }
    else if (strcmp(curCode->code.u.ifgotoOP.relop, "<=") == 0) {
        fprintf(file, "\tble %s, %s, label%d\n", regs[op1ID].name, regs[op2ID].name, curCode->code.u.ifgotoOP.label->var_no);
    }
    regs[op1ID].used = 0;
    regs[op2ID].used = 0;
    return;
}
void objcode_return(InterCodeList *curCode, FILE *file){
    fprintf(file, "\tlw $ra, 4($fp)\n");
    fprintf(file, "\taddi $sp, $fp, 8\n");
    int regID = get_reg(curCode->code.u.singleOP.op, file);
    fprintf(file, "\tlw $fp, 0($fp)\n");
    fprintf(file, "\tmove $v0, %s\n", regs[regID].name);
    fprintf(file, "\tjr $ra\n");
    deactivate_temp_regs();
    return;
}
void objcode_dec(InterCodeList *curCode, FILE *file){
    return;
}
void objcode_arg(InterCodeList *curCode, FILE *file){
    int argNum = 0;
    if (curCode->code.kind == ARG_INTERCODE) {
        while (curCode->code.kind == ARG_INTERCODE) {
            argNum++;
            fprintf(file, "\taddi $sp, $sp, -4\n");
            int free = get_reg(curCode->code.u.singleOP.op, file);
            fprintf(file, "\tsw %s, 0($sp)\n", regs[free].name);
            curCode = curCode->next;
            regs[free].used = 0;
        }
    }
    Operand *left = curCode->code.u.doubleOP.left;
    Operand *right = curCode->code.u.doubleOP.right;
    fprintf(file, "\tjal %s\n", right->funcName);
    fprintf(file, "\taddi $sp, $sp, %d\n", argNum * 4);
    int leftID = get_reg(left, file);
    fprintf(file, "\tmove %s, $v0\n", regs[leftID].name);
    save2mem(leftID, file);
    return;
}
void objcode_call(InterCodeList *curCode, FILE *file){
    return;
}
void objcode_param(InterCodeList *curCode, FILE *file){
    return;
}
void objcode_read(InterCodeList *curCode, FILE *file){
    fprintf(file, "\taddi $sp, $sp, -4\n");
    fprintf(file, "\tsw $ra, 0($sp)\n");
    fprintf(file, "\tjal read\n");
    fprintf(file, "\tlw $ra, 0($sp)\n");
    fprintf(file, "\taddi $sp, $sp, 4\n");
    int regID = get_reg(curCode->code.u.singleOP.op, file);
    fprintf(file, "\tmove %s, $v0\n", regs[regID].name);
    save2mem(regID, file);
    return;
}
void objcode_write(InterCodeList *curCode, FILE *file){
    int regID = get_reg(curCode->code.u.singleOP.op, file);
    fprintf(file, "\tmove $a0, %s\n", regs[regID].name);
    fprintf(file, "\taddi $sp, $sp, -4\n");
    fprintf(file, "\tsw $ra, 0($sp)\n");
    fprintf(file, "\tjal write\n");
    fprintf(file, "\tlw $ra, 0($sp)\n");
    fprintf(file, "\taddi $sp, $sp, 4\n");
    deactivate_temp_regs();
    return;
}


/*****************************************
 * Tool functions
 *****************************************/
/* initialize all regs, including setting name, reset used bit and varoffset */
void init_all_regs() {     
    for (int i = 0; i < 32; i++) {
        regs[i].name = regName[i];
        regs[i].used = 0;
        regs[i].var = NULL;
    }
}

void deactivate_temp_regs() {
    for (int i = 8; i < 16; i++) {
        regs[i].used = 0;
    }
}

/* get a register to fill operand */
int get_reg(Operand *op, FILE *file) {
    if (op->kind == CONSTANT_OP) {
        for (int i = 8; i < 16; i++) {
            if (regs[i].used == 0) {
                regs[i].used = 1;
                regs[i].var = NULL;
                fprintf(file, "\tli %s, %d\n", regs[i].name, op->value);
                return i;
            }
        }
    }

    for (int i = 8; i < 16; i++) {
        if (regs[i].used == 0) {
            regs[i].used = 1;
            regs[i].var = get_varoffset(op);
            regs[i].var->regNo = i;
            if (op->kind == TEMP_OP && op->type == ADDRESS) {
                /* *x */
                fprintf(file, "\tlw %s, %d($fp)\n", regs[i].name, regs[i].var->offset);
                fprintf(file, "\tlw %s, 0(%s)\n", regs[i].name, regs[i].name);
            }
            else if (op->kind == VARIABLE_OP && op->type == ADDRESS) {
                /* &x */
                fprintf(file, "\taddi %s, $fp, %d\n", regs[i].name, regs[i].var->offset);
            }
            else {
                /* x */
                fprintf(file, "\tlw %s, %d($fp)\n", regs[i].name, regs[i].var->offset);
            }
            return i;
        }
    }
    return 0;
}

VarOffset *get_varoffset(Operand *op)
{
    VarOffset *cur = headVarOffset;
    VarOffset *res = NULL;
    while (cur != NULL) {
        if (op->kind == VARIABLE_OP) {
            if (strcmp(cur->name, op->varName) == 0) {
                res = cur;
                return res;
            }
        }
        else if (op->kind == TEMP_OP) {
            char num[32];
            int2str(op->var_no, num);
            char name[32] = {'t'};
            strcat(name, num);
            if (strcmp(cur->name, name) == 0) {
                res = cur;
                return res;
            }
        }
        cur = cur->next;
    }
    return res;
}

void save2mem(int ID, FILE *file) {
    fprintf(file, "\tsw %s, %d($fp)\n", regs[ID].name, regs[ID].var->offset);
    deactivate_temp_regs();
}

void update_varoffset(Operand *op) {
    if (op->kind == CONSTANT_OP) {
        return;
    }

    VarOffset *tmpVO = get_varoffset(op);
    if (tmpVO == NULL) {
        localOffset += 4;
        tmpVO = (VarOffset *)malloc(sizeof(VarOffset));
        if (op->kind == VARIABLE_OP) {
            strcpy(tmpVO->name, op->varName);
        } 
        else if (op->kind == TEMP_OP) {
            char num[32];
            int2str(op->var_no, num);
            char name[32] = {'t'};
            strcat(name, num);
            strcpy(tmpVO->name, name);
        }
        tmpVO->offset = -localOffset;
        tmpVO->next = headVarOffset;
        headVarOffset = tmpVO;
    }
}

/* 其实一行sprintf就能在原本的函数中方便的实现
   不过为了提高过程的可读性，我们仍然采用int2str来进行格式转换 */
void int2str(int num, char *str) {
    sprintf(str, "%d", num);
}