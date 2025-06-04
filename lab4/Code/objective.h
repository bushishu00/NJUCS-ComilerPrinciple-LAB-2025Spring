#ifndef __OBJECTIVE_H__
#define __OBJECTIVE_H__

#include "intermediate.h"


typedef struct Register Register;
typedef struct VarOffset VarOffset;

struct Register {
    char *name;
    bool used;
    VarOffset *var;
};

struct VarOffset {
    char name[32];
    int regNo;
    int offset;
    VarOffset *next;
};

void generate_object_code(FILE *file);
void objcode_label(InterCodeList *curCode, FILE *file);
void objcode_func(InterCodeList *curCode, FILE *file);
void objcode_assign(InterCodeList *curCode, FILE *file); 
void objcode_add(InterCodeList *curCode, FILE *file);
void objcode_sub(InterCodeList *curCode, FILE *file);
void objcode_mul(InterCodeList *curCode, FILE *file); 
void objcode_div(InterCodeList *curCode, FILE *file);
void objcode_get_addr(InterCodeList *curCode, FILE *file);
void objcode_get_content(InterCodeList *curCode, FILE *file);
void objcode_to_addr(InterCodeList *curCode, FILE *file);
void objcode_goto(InterCodeList *curCode, FILE *file);
void objcode_ifgoto(InterCodeList *curCode, FILE *file);
void objcode_return(InterCodeList *curCode, FILE *file);
void objcode_dec(InterCodeList *curCode, FILE *file);
void objcode_arg(InterCodeList *curCode, FILE *file);
void objcode_call(InterCodeList *curCode, FILE *file);
void objcode_param(InterCodeList *curCode, FILE *file);
void objcode_read(InterCodeList *curCode, FILE *file);
void objcode_write(InterCodeList *curCode, FILE *file);

/* some tool functions */
void init_all_regs();
int get_reg(Operand *op, FILE *file);
VarOffset *get_varoffset(Operand *op);
void int2str(int num, char *str);
void save2mem(int ID, FILE *file);
void update_varoffset(Operand *op);
void deactivate_temp_regs();
#endif